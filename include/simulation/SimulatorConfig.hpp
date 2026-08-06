#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include <nlohmann/json_fwd.hpp>

#include "simulation/FailureScenario.hpp"

namespace evoarch
{
    struct SimulatorConfig
    {
        std::string name;

        std::string description;

        std::uint32_t seed = 42;

        double durationSeconds = 60.0;

        double requestsPerSecond = 100.0;

        FailureScenario failureScenario;
    };

    class SimulatorConfigLoader
    {
    public:

        static SimulatorConfig loadFromFile(const std::filesystem::path& path);

        static SimulatorConfig loadFromJson(const nlohmann::json& json);
    };
}
