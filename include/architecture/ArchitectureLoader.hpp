#pragma once

#include <filesystem>
#include <string>

#include <nlohmann/json_fwd.hpp>

#include "architecture/Architecture.hpp"

namespace evoarch
{
    struct ArchitectureDefinition
    {
        std::string name;

        std::string version;

        std::string description;

        Architecture architecture;
    };

    class ArchitectureLoader
    {
    public:

        static ArchitectureDefinition loadFromFile(const std::filesystem::path& path);

        static ArchitectureDefinition loadFromJson(const nlohmann::json& json);
    };
}
