#include "services/ApiService.hpp"

namespace evoarch
{
    PerformanceModel ApiService::defaultPerformanceModel()
    {
        return PerformanceModel{
            .processing = ProcessingTimeModel{
                .distribution = ProcessingDistribution::Normal,
                .meanMs = 5.0,
                .stdDevMs = 1.5,
            },
            .maxConcurrentRequests = 64,
            .monthlyCost = 80.0,
        };
    }
}
