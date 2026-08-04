#include "simulation/Metrics.hpp"

#include <iostream>
#include <iomanip>

namespace evoarch
{
    void Metrics::reset()
    {
        m_completedRequests = 0;
        m_failedRequests = 0;
        m_averageLatencyMs = 0.0;
        m_p95LatencyMs = 0.0;
        m_p99LatencyMs = 0.0;
        m_throughput = 0.0;
        m_availability = 0.0;
        m_monthlyCost = 0.0;
        m_queueLengths.clear();
        m_componentUtilization.clear();
    }

    void Metrics::setCompletedRequests(std::size_t count)
    {
        m_completedRequests = count;
    }

    void Metrics::setFailedRequests(std::size_t count)
    {
        m_failedRequests = count;
    }

    void Metrics::setAverageLatencyMs(double latencyMs)
    {
        m_averageLatencyMs = latencyMs;
    }

    void Metrics::setP95LatencyMs(double latencyMs)
    {
        m_p95LatencyMs = latencyMs;
    }

    void Metrics::setP99LatencyMs(double latencyMs)
    {
        m_p99LatencyMs = latencyMs;
    }

    void Metrics::setThroughput(double requestsPerSecond)
    {
        m_throughput = requestsPerSecond;
    }

    void Metrics::setAvailability(double fraction)
    {
        m_availability = fraction;
    }

    void Metrics::setMonthlyCost(double cost)
    {
        m_monthlyCost = cost;
    }

    void Metrics::setQueueLength(
        const std::string& componentId,
        double length)
    {
        m_queueLengths[componentId] = length;
    }

    void Metrics::setComponentUtilization(
        const std::string& componentId,
        double utilization)
    {
        m_componentUtilization[componentId] = utilization;
    }

    std::size_t Metrics::completedRequests() const
    {
        return m_completedRequests;
    }

    std::size_t Metrics::failedRequests() const
    {
        return m_failedRequests;
    }

    std::size_t Metrics::totalRequests() const
    {
        return m_completedRequests + m_failedRequests;
    }

    double Metrics::averageLatencyMs() const
    {
        return m_averageLatencyMs;
    }

    double Metrics::p95LatencyMs() const
    {
        return m_p95LatencyMs;
    }

    double Metrics::p99LatencyMs() const
    {
        return m_p99LatencyMs;
    }

    double Metrics::throughput() const
    {
        return m_throughput;
    }

    double Metrics::availability() const
    {
        return m_availability;
    }

    double Metrics::monthlyCost() const
    {
        return m_monthlyCost;
    }

    const std::unordered_map<std::string, double>& Metrics::queueLengths() const
    {
        return m_queueLengths;
    }

    const std::unordered_map<std::string, double>& Metrics::componentUtilization() const
    {
        return m_componentUtilization;
    }

    void Metrics::print() const
    {
        std::cout << std::fixed << std::setprecision(2);

        std::cout << "Metrics\n\n";

        std::cout << "Requests\n";
        std::cout << "  Completed: " << m_completedRequests << "\n";
        std::cout << "  Failed: " << m_failedRequests << "\n";
        std::cout << "  Total: " << totalRequests() << "\n\n";

        std::cout << "Latency\n";
        std::cout << "  Average: " << m_averageLatencyMs << " ms\n";
        std::cout << "  P95: " << m_p95LatencyMs << " ms\n";
        std::cout << "  P99: " << m_p99LatencyMs << " ms\n\n";

        std::cout << "Throughput: " << m_throughput << " req/s\n\n";

        std::cout << "Availability: " << (m_availability * 100.0) << "%\n";
        std::cout << "Monthly cost: $" << m_monthlyCost << "\n\n";

        if (!m_queueLengths.empty())
        {
            std::cout << "Queue lengths\n";

            for (const auto& [componentId, length] : m_queueLengths)
            {
                std::cout << "  " << componentId << ": " << length << "\n";
            }

            std::cout << "\n";
        }

        if (!m_componentUtilization.empty())
        {
            std::cout << "Component utilization\n";

            for (const auto& [componentId, utilization] : m_componentUtilization)
            {
                std::cout << "  " << componentId << ": " << (utilization * 100.0) << "%\n";
            }
        }
    }
}
