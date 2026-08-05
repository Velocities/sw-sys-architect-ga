#include "services/LoadBalancer.hpp"

namespace evoarch
{
    PerformanceModel LoadBalancer::defaultPerformanceModel()
    {
        return PerformanceModel{
            .processing = ProcessingTimeModel{
                .distribution = ProcessingDistribution::Fixed,
                .meanMs = 0.1,
                .stdDevMs = 0.0,
            },
            .maxConcurrentRequests = 10000,
            .monthlyCost = 25.0,
        };
    }
}
