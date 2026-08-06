#include "simulation/SimulatorConfig.hpp"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace evoarch
{
    namespace
    {
        std::string requireStringField(
            const nlohmann::json& json,
            const char* fieldName,
            const char* context)
        {
            if (!json.contains(fieldName) || !json[fieldName].is_string())
            {
                throw std::runtime_error(
                    std::string(context) + ": missing or invalid string field '" + fieldName + "'");
            }

            return json[fieldName].get<std::string>();
        }

        FailureScenario parseFailureScenario(
            const nlohmann::json& json,
            const nlohmann::json& root)
        {
            FailureScenario scenario;

            if (json.contains("preset") && json["preset"].is_string())
            {
                scenario = FailureScenario(json["preset"].get<std::string>());
            }

            if (!json.contains("events") || !json["events"].is_array())
            {
                return scenario;
            }

            for (const auto& eventJson : json["events"])
            {
                FailureEvent event;

                event.type = parseFailureEventType(
                    requireStringField(eventJson, "type", "Failure event"));
                event.targetComponentId =
                    requireStringField(eventJson, "target", "Failure event");

                if (!eventJson.contains("at_time_ms") || !eventJson["at_time_ms"].is_number())
                {
                    throw std::runtime_error("Failure event: missing or invalid 'at_time_ms'");
                }

                event.atTimeMs = eventJson["at_time_ms"].get<double>();

                if (eventJson.contains("duration_ms") && eventJson["duration_ms"].is_number())
                {
                    event.durationMs = eventJson["duration_ms"].get<double>();
                }

                scenario.addEvent(std::move(event));
            }

            if (json.contains("preset") && json["preset"].is_string())
            {
                const std::string preset = json["preset"].get<std::string>();

                if (preset == "kill_primary_db" && scenario.events().empty())
                {
                    FailureEvent event;
                    event.type = FailureEventType::NodeDown;
                    event.targetComponentId = "db-primary";
                    event.atTimeMs = root.value("duration_seconds", 60.0) * 1000.0 * 0.5;
                    event.durationMs = root.value("duration_seconds", 60.0) * 1000.0 * 0.25;
                    scenario.addEvent(std::move(event));
                }
            }

            return scenario;
        }
    }

    SimulatorConfig SimulatorConfigLoader::loadFromFile(const std::filesystem::path& path)
    {
        std::ifstream input(path);

        if (!input)
        {
            throw std::runtime_error("Failed to open simulator config: " + path.string());
        }

        nlohmann::json json;

        try
        {
            input >> json;
        }
        catch (const nlohmann::json::exception& exception)
        {
            throw std::runtime_error(
                "Failed to parse simulator JSON in " + path.string() + ": " + exception.what());
        }

        return loadFromJson(json);
    }

    SimulatorConfig SimulatorConfigLoader::loadFromJson(const nlohmann::json& json)
    {
        SimulatorConfig config;

        config.name = requireStringField(json, "name", "Simulator JSON");
        config.description = requireStringField(json, "description", "Simulator JSON");
        config.seed = json.value("seed", 42U);
        config.durationSeconds = json.value("duration_seconds", 60.0);
        config.requestsPerSecond = json.value("requests_per_second", 100.0);

        if (json.contains("failure_injection") && json["failure_injection"].is_object())
        {
            const auto& failureInjection = json["failure_injection"];

            if (failureInjection.value("enabled", false))
            {
                config.failureScenario = parseFailureScenario(failureInjection, json);
            }
        }

        return config;
    }
}
