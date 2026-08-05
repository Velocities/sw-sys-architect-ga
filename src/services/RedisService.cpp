#include "services/RedisService.hpp"

namespace evoarch
{
    PerformanceModel RedisService::defaultPerformanceModel()
    {
        return PerformanceModel{
            .processing = ProcessingTimeModel{
                .distribution = ProcessingDistribution::Normal,
                .meanMs = 1.0,
                .stdDevMs = 0.2,
            },
            .maxConcurrentRequests = 256,
            .monthlyCost = 50.0,
        };
    }
}
