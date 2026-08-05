#include "services/PostgresService.hpp"

namespace evoarch
{
    PerformanceModel PostgresService::defaultPerformanceModel()
    {
        return PerformanceModel{
            .processing = ProcessingTimeModel{
                .distribution = ProcessingDistribution::LogNormal,
                .meanMs = 15.0,
                .stdDevMs = 5.0,
            },
            .maxConcurrentRequests = 32,
            .monthlyCost = 120.0,
        };
    }
}
