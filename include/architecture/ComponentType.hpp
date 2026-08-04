#pragma once

#include <string_view>

namespace evoarch
{
    enum class ComponentType
    {
        Api,
        Redis,
        Postgres,
        LoadBalancer,

        Unknown
    };

    inline std::string_view componentTypeName(ComponentType type)
    {
        switch (type)
        {
        case ComponentType::Api:
            return "API";
        case ComponentType::Redis:
            return "Redis";
        case ComponentType::Postgres:
            return "PostgreSQL";
        case ComponentType::LoadBalancer:
            return "LoadBalancer";
        case ComponentType::Unknown:
            return "Unknown";
        }

        return "Unknown";
    }
}