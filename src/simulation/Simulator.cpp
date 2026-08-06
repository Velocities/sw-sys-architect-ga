#include "simulation/Simulator.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/range/iterator_range.hpp>

#include "architecture/Component.hpp"
#include "architecture/PerformanceModel.hpp"
#include "simulation/ComponentRuntimeState.hpp"
#include "simulation/Event.hpp"
#include "simulation/Request.hpp"

namespace evoarch
{
    namespace
    {
        // Accumulators filled during the run and converted into Metrics at the end.
        struct RuntimeMetrics
        {
            std::size_t completedRequests = 0;

            std::size_t failedRequests = 0;

            std::vector<double> completedLatencies;

            std::unordered_map<Architecture::Vertex, double> totalProcessingTimeMs;

            std::unordered_map<Architecture::Vertex, double> peakQueueLength;
        };

        bool isClientHop(const WorkflowHop& hop)
        {
            return std::holds_alternative<WorkflowEndpoint>(hop);
        }

        std::optional<ComponentType> hopComponentType(const WorkflowHop& hop)
        {
            if (const auto* componentType = std::get_if<ComponentType>(&hop))
            {
                return *componentType;
            }

            return std::nullopt;
        }

        std::optional<Architecture::Vertex> findVertexByComponentId(
            const Architecture& architecture,
            const std::string& componentId)
        {
            for (auto vertex :
                 boost::make_iterator_range(boost::vertices(architecture.graph())))
            {
                const auto& component = architecture.graph()[vertex].component;

                if (component && component->id() == componentId)
                {
                    return vertex;
                }
            }

            return std::nullopt;
        }

        std::vector<Architecture::Vertex> findVerticesByType(
            const Architecture& architecture,
            ComponentType type)
        {
            std::vector<Architecture::Vertex> vertices;

            for (auto vertex :
                 boost::make_iterator_range(boost::vertices(architecture.graph())))
            {
                const auto& component = architecture.graph()[vertex].component;

                if (component && component->type() == type)
                {
                    vertices.push_back(vertex);
                }
            }

            return vertices;
        }

        std::optional<double> findEdgeLatencyMs(
            const Architecture& architecture,
            Architecture::Vertex from,
            Architecture::Vertex to)
        {
            const auto [edge, exists] = boost::edge(from, to, architecture.graph());

            if (!exists)
            {
                return std::nullopt;
            }

            return architecture.graph()[edge].latencyMs;
        }

        double sampleProcessingTimeMs(
            const PerformanceModel& model,
            std::mt19937& randomEngine)
        {
            // Draw a single service-time sample from the component's configured distribution.
            switch (model.processing.distribution)
            {
            case ProcessingDistribution::Fixed:
                return model.processing.meanMs;

            case ProcessingDistribution::Normal:
            {
                std::normal_distribution<double> distribution(
                    model.processing.meanMs,
                    model.processing.stdDevMs);

                return std::max(0.0, distribution(randomEngine));
            }

            case ProcessingDistribution::LogNormal:
            {
                const double variance =
                    model.processing.stdDevMs * model.processing.stdDevMs;
                const double mean = model.processing.meanMs;

                if (mean <= 0.0)
                {
                    return 0.0;
                }

                const double sigmaSquared = std::log1p(variance / (mean * mean));
                const double mu = std::log(mean) - (sigmaSquared / 2.0);
                const double sigma = std::sqrt(sigmaSquared);

                std::lognormal_distribution<double> distribution(mu, sigma);
                return distribution(randomEngine);
            }
            }

            return model.processing.meanMs;
        }

        std::size_t chooseWorkflowIndex(
            const Workload& workload,
            std::mt19937& randomEngine)
        {
            // Weighted random choice matching each workflow's traffic percentage.
            std::uniform_real_distribution<double> distribution(0.0, 100.0);
            const double roll = distribution(randomEngine);

            double cumulative = 0.0;

            for (std::size_t index = 0; index < workload.workflows().size(); ++index)
            {
                cumulative += workload.workflows()[index].percentage;

                if (roll <= cumulative)
                {
                    return index;
                }
            }

            return workload.workflows().size() - 1;
        }

        std::size_t firstComponentHopIndex(const std::vector<WorkflowHop>& hops)
        {
            for (std::size_t index = 0; index < hops.size(); ++index)
            {
                if (hopComponentType(hops[index]).has_value())
                {
                    return index;
                }
            }

            return hops.size();
        }

        bool advanceToNextComponentHop(Request& request)
        {
            // Skip Client hops until the next real component type (or end of workflow).
            if (request.hopIndex() + 1 >= request.hops().size())
            {
                return false;
            }

            request.setHopIndex(request.hopIndex() + 1);

            while (request.hopIndex() < request.hops().size())
            {
                if (hopComponentType(request.hops()[request.hopIndex()]).has_value())
                {
                    return true;
                }

                if (request.hopIndex() + 1 >= request.hops().size())
                {
                    return false;
                }

                request.setHopIndex(request.hopIndex() + 1);
            }

            return false;
        }

        double percentileFromSorted(
            const std::vector<double>& values,
            double percentile)
        {
            if (values.empty())
            {
                return 0.0;
            }

            const double rank = percentile * static_cast<double>(values.size() - 1);
            const std::size_t lowerIndex = static_cast<std::size_t>(std::floor(rank));
            const std::size_t upperIndex = static_cast<std::size_t>(std::ceil(rank));

            if (lowerIndex == upperIndex)
            {
                return values[lowerIndex];
            }

            const double weight = rank - static_cast<double>(lowerIndex);
            return values[lowerIndex] + (weight * (values[upperIndex] - values[lowerIndex]));
        }

        double computeMonthlyCost(const Architecture& architecture)
        {
            // Static architecture cost rollup used by fitness functions later.
            double totalCost = 0.0;

            for (auto vertex :
                 boost::make_iterator_range(boost::vertices(architecture.graph())))
            {
                const auto& component = architecture.graph()[vertex].component;

                if (component)
                {
                    totalCost += component->model().monthlyCost;
                }
            }

            return totalCost;
        }
    }

    Simulator::Simulator(
        Architecture architecture,
        Workload workload,
        SimulatorConfig config)
        :
        m_architecture(std::move(architecture)),
        m_workload(std::move(workload)),
        m_config(std::move(config))
    {
    }

    Metrics Simulator::run()
    {
        Metrics metrics;
        metrics.reset();
        metrics.setMonthlyCost(computeMonthlyCost(m_architecture));

        if (m_workload.workflowCount() == 0)
        {
            return metrics;
        }

        // DES state: seeded RNG, time-ordered event queue, live requests, per-vertex runtime.
        std::mt19937 randomEngine(m_config.seed);
        EventQueue eventQueue;
        std::unordered_map<RequestId, Request> activeRequests;
        std::unordered_map<Architecture::Vertex, ComponentRuntimeState> runtimeStates;
        RuntimeMetrics runtimeMetrics;

        for (auto vertex :
             boost::make_iterator_range(boost::vertices(m_architecture.graph())))
        {
            runtimeStates.emplace(vertex, ComponentRuntimeState{});
        }

        const double durationMs = m_config.durationSeconds * 1000.0;
        const double arrivalIntervalMs =
            m_config.requestsPerSecond > 0.0 ? (1000.0 / m_config.requestsPerSecond) : 0.0;

        const auto scheduleFailureEvents = [&]()
        {
            // Pre-schedule failure/recovery events onto the same queue as request traffic.
            for (const FailureEvent& failureEvent : m_config.failureScenario.events())
            {
                SimulationEvent event;
                event.timeMs = failureEvent.atTimeMs;
                event.type = SimulationEventType::FailureInjection;
                event.payload = FailureInjectionPayload{failureEvent};
                eventQueue.push(event);

                if (failureEvent.durationMs.has_value()
                    && failureEvent.type == FailureEventType::NodeDown)
                {
                    FailureEvent recoveryEvent;
                    recoveryEvent.type = FailureEventType::NodeRecovery;
                    recoveryEvent.targetComponentId = failureEvent.targetComponentId;
                    recoveryEvent.atTimeMs =
                        failureEvent.atTimeMs + failureEvent.durationMs.value();

                    SimulationEvent recovery;
                    recovery.timeMs = recoveryEvent.atTimeMs;
                    recovery.type = SimulationEventType::FailureInjection;
                    recovery.payload = FailureInjectionPayload{recoveryEvent};
                    eventQueue.push(recovery);
                }
            }
        };

        const auto resolveTargetVertex =
            [&](const std::string& targetId) -> std::optional<Architecture::Vertex>
        {
            if (auto vertex = findVertexByComponentId(m_architecture, targetId))
            {
                return vertex;
            }

            if (targetId == "db-primary")
            {
                const auto postgresVertices =
                    findVerticesByType(m_architecture, ComponentType::Postgres);

                if (!postgresVertices.empty())
                {
                    return postgresVertices.front();
                }
            }

            return std::nullopt;
        };

        const auto routeToComponentType = [&](
                                              Architecture::Vertex currentVertex,
                                              ComponentType type) -> std::optional<Architecture::Vertex>
        {
            // Prefer a direct outgoing edge; otherwise auto-route to any healthy node of that type.
            for (auto edge :
                 boost::make_iterator_range(boost::out_edges(currentVertex, m_architecture.graph())))
            {
                const auto target = boost::target(edge, m_architecture.graph());
                const auto& component = m_architecture.graph()[target].component;

                if (component && component->type() == type && runtimeStates[target].isHealthy())
                {
                    return target;
                }
            }

            for (Architecture::Vertex candidate :
                 findVerticesByType(m_architecture, type))
            {
                if (runtimeStates[candidate].isHealthy())
                {
                    return candidate;
                }
            }

            return std::nullopt;
        };

        const auto failRequest = [&](Request& request)
        {
            request.setFailed(true);
            ++runtimeMetrics.failedRequests;
            activeRequests.erase(request.id());
        };

        const auto completeRequest = [&](const Request& request)
        {
            ++runtimeMetrics.completedRequests;
            runtimeMetrics.completedLatencies.push_back(request.totalLatency());
            activeRequests.erase(request.id());
        };

        std::function<void(Request&)> beginProcessing;

        const auto tryStartQueuedRequest = [&](Architecture::Vertex vertex)
        {
            // When capacity frees up, drain the waiting queue at this component.
            auto& state = runtimeStates[vertex];
            const auto& component = m_architecture.graph()[vertex].component;

            if (!component || !state.isHealthy())
            {
                return;
            }

            while (state.inFlight() < component->model().maxConcurrentRequests)
            {
                const auto queuedRequestId = state.dequeue();

                if (!queuedRequestId.has_value())
                {
                    break;
                }

                const auto requestIt = activeRequests.find(queuedRequestId.value());

                if (requestIt == activeRequests.end())
                {
                    continue;
                }

                beginProcessing(requestIt->second);
            }
        };

        beginProcessing = [&](Request& request)
        {
            const Architecture::Vertex vertex = request.currentVertex();
            auto& state = runtimeStates[vertex];
            const auto& component = m_architecture.graph()[vertex].component;

            if (!component)
            {
                failRequest(request);
                return;
            }

            if (!state.isHealthy())
            {
                // Failover: reroute to another healthy instance of the same component type.
                const auto rerouted = routeToComponentType(vertex, component->type());

                if (!rerouted.has_value())
                {
                    failRequest(request);
                    return;
                }

                const double networkLatency =
                    findEdgeLatencyMs(m_architecture, vertex, rerouted.value()).value_or(1.0);
                request.addLatency(networkLatency);
                request.setCurrentVertex(rerouted.value());
                beginProcessing(request);
                return;
            }

            if (state.inFlight() >= component->model().maxConcurrentRequests)
            {
                // At capacity — wait until a processing slot opens.
                state.enqueue(request.id());
                runtimeMetrics.peakQueueLength[vertex] = std::max(
                    runtimeMetrics.peakQueueLength[vertex],
                    static_cast<double>(state.queued()));
                return;
            }

            state.incrementInFlight();

            const double processingTimeMs =
                sampleProcessingTimeMs(component->model(), randomEngine);
            runtimeMetrics.totalProcessingTimeMs[vertex] += processingTimeMs;

            // Schedule when this request will finish at the current component.
            SimulationEvent completionEvent;
            completionEvent.timeMs = request.currentTime() + processingTimeMs;
            completionEvent.type = SimulationEventType::ProcessingComplete;
            completionEvent.payload =
                ProcessingCompletePayload{request.id(), request.currentVertex()};
            eventQueue.push(completionEvent);
        };

        const auto moveRequestToNextHop = [&](Request& request)
        {
            if (!advanceToNextComponentHop(request))
            {
                completeRequest(request);
                return;
            }

            // Route to the next workflow hop and continue processing there.
            const auto nextType = hopComponentType(request.hops()[request.hopIndex()]);

            if (!nextType.has_value())
            {
                failRequest(request);
                return;
            }

            const auto nextVertex =
                routeToComponentType(request.currentVertex(), nextType.value());

            if (!nextVertex.has_value())
            {
                failRequest(request);
                return;
            }

            const double networkLatency = findEdgeLatencyMs(
                                              m_architecture,
                                              request.currentVertex(),
                                              nextVertex.value())
                                              .value_or(1.0);

            request.addLatency(networkLatency);
            request.setCurrentTime(request.currentTime() + networkLatency);
            request.setCurrentVertex(nextVertex.value());
            beginProcessing(request);
        };

        const auto spawnRequest = [&](std::size_t workflowIndex, double arrivalTimeMs)
        {
            // Create a new request at the workflow's first real component hop.
            const Workflow& workflow = m_workload.workflows()[workflowIndex].workflow;
            const std::vector<WorkflowHop>& hops = workflow.hops();
            const std::size_t hopIndex = firstComponentHopIndex(hops);

            if (hopIndex >= hops.size())
            {
                return;
            }

            const auto componentType = hopComponentType(hops[hopIndex]);

            if (!componentType.has_value())
            {
                return;
            }

            const auto entryVertices = findVerticesByType(m_architecture, componentType.value());

            if (entryVertices.empty())
            {
                return;
            }

            Architecture::Vertex entryVertex = entryVertices.front();

            for (Architecture::Vertex candidate : entryVertices)
            {
                if (runtimeStates[candidate].isHealthy())
                {
                    entryVertex = candidate;
                    break;
                }
            }

            Request request(
                RequestId::generate(),
                hops,
                entryVertex,
                arrivalTimeMs);
            request.setHopIndex(hopIndex);

            auto [iterator, inserted] =
                activeRequests.emplace(request.id(), std::move(request));

            if (!inserted)
            {
                return;
            }

            beginProcessing(iterator->second);
        };

        scheduleFailureEvents();

        if (arrivalIntervalMs > 0.0)
        {
            // Fixed-interval arrivals for the whole run; each picks a workflow by weight.
            for (double arrivalTimeMs = 0.0; arrivalTimeMs < durationMs;
                 arrivalTimeMs += arrivalIntervalMs)
            {
                SimulationEvent arrivalEvent;
                arrivalEvent.timeMs = arrivalTimeMs;
                arrivalEvent.type = SimulationEventType::RequestArrival;
                arrivalEvent.payload = RequestArrivalPayload{
                    chooseWorkflowIndex(m_workload, randomEngine),
                };
                eventQueue.push(arrivalEvent);
            }
        }

        while (!eventQueue.empty())
        {
            // std::priority_queue::pop() removes the top element but returns void —
            // top() must be called first to read the next event before removing it.
            SimulationEvent event = eventQueue.top();
            eventQueue.pop();

            if (event.timeMs > durationMs)
            {
                continue;
            }

            switch (event.type)
            {
            case SimulationEventType::RequestArrival:
            {
                const auto& payload = std::get<RequestArrivalPayload>(event.payload);
                spawnRequest(payload.workflowIndex, event.timeMs);
                break;
            }

            case SimulationEventType::ProcessingComplete:
            {
                // Service finished — advance the workflow and free capacity for queued work.
                const auto& payload = std::get<ProcessingCompletePayload>(event.payload);
                const auto requestIt = activeRequests.find(payload.requestId);

                if (requestIt == activeRequests.end())
                {
                    break;
                }

                Request& request = requestIt->second;
                auto& state = runtimeStates[payload.vertex];
                state.decrementInFlight();

                const double completionTimeMs = event.timeMs;
                const double processingDurationMs = completionTimeMs - request.currentTime();
                request.addLatency(processingDurationMs);
                request.setCurrentTime(completionTimeMs);

                moveRequestToNextHop(request);
                tryStartQueuedRequest(payload.vertex);
                break;
            }

            case SimulationEventType::FailureInjection:
            {
                // Scenario-driven health changes (used for redundancy/resilience testing).
                const auto& payload = std::get<FailureInjectionPayload>(event.payload);
                const auto targetVertex = resolveTargetVertex(payload.failureEvent.targetComponentId);

                if (!targetVertex.has_value())
                {
                    break;
                }

                if (payload.failureEvent.type == FailureEventType::NodeDown)
                {
                    runtimeStates[targetVertex.value()].setHealthy(false);
                }
                else
                {
                    runtimeStates[targetVertex.value()].setHealthy(true);
                    tryStartQueuedRequest(targetVertex.value());
                }

                break;
            }
            }
        }

        // Convert runtime counters into the final Metrics snapshot.
        metrics.setCompletedRequests(runtimeMetrics.completedRequests);
        metrics.setFailedRequests(runtimeMetrics.failedRequests);

        if (!runtimeMetrics.completedLatencies.empty())
        {
            std::vector<double> sortedLatencies = runtimeMetrics.completedLatencies;
            std::sort(sortedLatencies.begin(), sortedLatencies.end());

            double totalLatency = 0.0;

            for (double latency : sortedLatencies)
            {
                totalLatency += latency;
            }

            metrics.setAverageLatencyMs(totalLatency / static_cast<double>(sortedLatencies.size()));
            metrics.setP95LatencyMs(percentileFromSorted(sortedLatencies, 0.95));
            metrics.setP99LatencyMs(percentileFromSorted(sortedLatencies, 0.99));
        }

        if (m_config.durationSeconds > 0.0)
        {
            metrics.setThroughput(static_cast<double>(runtimeMetrics.completedRequests)
                                  / m_config.durationSeconds);
        }

        const std::size_t totalRequests =
            runtimeMetrics.completedRequests + runtimeMetrics.failedRequests;

        if (totalRequests > 0)
        {
            metrics.setAvailability(static_cast<double>(runtimeMetrics.completedRequests)
                                    / static_cast<double>(totalRequests));
        }
        else
        {
            metrics.setAvailability(1.0);
        }

        for (auto vertex :
             boost::make_iterator_range(boost::vertices(m_architecture.graph())))
        {
            const auto& component = m_architecture.graph()[vertex].component;

            if (!component)
            {
                continue;
            }

            // Per-component queue and utilization snapshots for the fitness function.
            const double peakQueue = runtimeMetrics.peakQueueLength.contains(vertex)
                ? runtimeMetrics.peakQueueLength[vertex]
                : static_cast<double>(runtimeStates[vertex].queued());

            metrics.setQueueLength(component->id(), peakQueue);

            const double processingTime = runtimeMetrics.totalProcessingTimeMs.contains(vertex)
                ? runtimeMetrics.totalProcessingTimeMs[vertex]
                : 0.0;
            const double capacityMs =
                durationMs * static_cast<double>(component->model().maxConcurrentRequests);
            const double utilization =
                capacityMs > 0.0 ? processingTime / capacityMs : 0.0;

            metrics.setComponentUtilization(component->id(), utilization);
        }

        return metrics;
    }
}
