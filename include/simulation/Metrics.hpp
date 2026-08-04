#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

namespace evoarch
{
    // Simulation output produced after a run. The simulator measures; downstream
    // consumers (reports, fitness functions) evaluate these values.
    class Metrics
    {
    public:

        void reset();

        void setCompletedRequests(std::size_t count);

        void setFailedRequests(std::size_t count);

        void setAverageLatencyMs(double latencyMs);

        void setP95LatencyMs(double latencyMs);

        void setP99LatencyMs(double latencyMs);

        void setThroughput(double requestsPerSecond);

        void setAvailability(double fraction);

        void setMonthlyCost(double cost);

        void setQueueLength(
            const std::string& componentId,
            double length);

        void setComponentUtilization(
            const std::string& componentId,
            double utilization);

        std::size_t completedRequests() const;

        std::size_t failedRequests() const;

        std::size_t totalRequests() const;

        double averageLatencyMs() const;

        double p95LatencyMs() const;

        double p99LatencyMs() const;

        double throughput() const;

        double availability() const;

        double monthlyCost() const;

        const std::unordered_map<std::string, double>& queueLengths() const;

        const std::unordered_map<std::string, double>& componentUtilization() const;

        void print() const;

    private:

        std::size_t m_completedRequests = 0;

        std::size_t m_failedRequests = 0;

        double m_averageLatencyMs = 0.0;

        double m_p95LatencyMs = 0.0;

        double m_p99LatencyMs = 0.0;

        double m_throughput = 0.0;

        double m_availability = 0.0;

        double m_monthlyCost = 0.0;

        std::unordered_map<std::string, double> m_queueLengths;

        std::unordered_map<std::string, double> m_componentUtilization;
    };
}
