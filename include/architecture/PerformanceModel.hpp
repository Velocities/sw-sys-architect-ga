#pragma once

#include <cstddef>
#include <string>

#include "architecture/ComponentType.hpp"

namespace evoarch
{
    enum class ProcessingDistribution
    {
        Fixed,
        Normal,
        LogNormal
    };

    std::string processingDistributionName(ProcessingDistribution distribution);

    ProcessingDistribution parseProcessingDistribution(const std::string& name);

    struct ProcessingTimeModel
    {
        ProcessingDistribution distribution = ProcessingDistribution::Fixed;

        double meanMs = 0.0;

        double stdDevMs = 0.0;
    };

    struct PerformanceModel
    {
        ProcessingTimeModel processing;

        std::size_t maxConcurrentRequests = 1;

        double monthlyCost = 0.0;
    };

    PerformanceModel defaultPerformanceModelForType(ComponentType type);
}
