#include "architecture/PerformanceModel.hpp"

#include <stdexcept>

#include "services/ApiService.hpp"
#include "services/LoadBalancer.hpp"
#include "services/PostgresService.hpp"
#include "services/RedisService.hpp"

namespace evoarch
{
    std::string processingDistributionName(ProcessingDistribution distribution)
    {
        switch (distribution)
        {
        case ProcessingDistribution::Fixed:
            return "fixed";
        case ProcessingDistribution::Normal:
            return "normal";
        case ProcessingDistribution::LogNormal:
            return "log_normal";
        }

        return "unknown";
    }

    ProcessingDistribution parseProcessingDistribution(const std::string& name)
    {
        if (name == "fixed")
        {
            return ProcessingDistribution::Fixed;
        }

        if (name == "normal")
        {
            return ProcessingDistribution::Normal;
        }

        if (name == "log_normal" || name == "lognormal")
        {
            return ProcessingDistribution::LogNormal;
        }

        throw std::runtime_error("Unknown processing distribution: " + name);
    }

    PerformanceModel defaultPerformanceModelForType(ComponentType type)
    {
        switch (type)
        {
        case ComponentType::Api:
            return ApiService::defaultPerformanceModel();
        case ComponentType::Redis:
            return RedisService::defaultPerformanceModel();
        case ComponentType::Postgres:
            return PostgresService::defaultPerformanceModel();
        case ComponentType::LoadBalancer:
            return LoadBalancer::defaultPerformanceModel();
        case ComponentType::Unknown:
            break;
        }

        return PerformanceModel{};
    }
}
