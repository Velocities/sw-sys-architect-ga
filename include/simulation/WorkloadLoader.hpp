#pragma once

#include <filesystem>

#include <nlohmann/json_fwd.hpp>

#include "simulation/Workload.hpp"

namespace evoarch
{
    class WorkloadLoader
    {
    public:

        static Workload loadFromFile(const std::filesystem::path& path);

        static Workload loadFromJson(const nlohmann::json& json);
    };
}
