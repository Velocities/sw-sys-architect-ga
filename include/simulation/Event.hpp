#pragma once

#include <cstddef>
#include <functional>
#include <queue>
#include <variant>

#include "architecture/Architecture.hpp"
#include "simulation/FailureScenario.hpp"
#include "simulation/RequestId.hpp"
#include "simulation/Workflow.hpp"

namespace evoarch
{
    enum class SimulationEventType
    {
        RequestArrival,
        ProcessingComplete,
        FailureInjection
    };

    struct RequestArrivalPayload
    {
        std::size_t workflowIndex = 0;
    };

    struct ProcessingCompletePayload
    {
        RequestId requestId;

        Architecture::Vertex vertex = 0;
    };

    struct FailureInjectionPayload
    {
        FailureEvent failureEvent;
    };

    using SimulationEventPayload = std::variant<
        RequestArrivalPayload,
        ProcessingCompletePayload,
        FailureInjectionPayload>;

    struct SimulationEvent
    {
        double timeMs = 0.0;

        SimulationEventType type = SimulationEventType::RequestArrival;

        SimulationEventPayload payload;

        bool operator>(const SimulationEvent& other) const
        {
            return timeMs > other.timeMs;
        }
    };

    using EventQueue = std::priority_queue<
        SimulationEvent,
        std::vector<SimulationEvent>,
        std::greater<SimulationEvent>>;
}
