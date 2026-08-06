#include "simulation/WorkloadLoader.hpp"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "simulation/Workflow.hpp"

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
    }

    Workload WorkloadLoader::loadFromFile(const std::filesystem::path& path)
    {
        std::ifstream input(path);

        if (!input)
        {
            throw std::runtime_error("Failed to open workload file: " + path.string());
        }

        nlohmann::json json;

        try
        {
            input >> json;
        }
        catch (const nlohmann::json::exception& exception)
        {
            throw std::runtime_error(
                "Failed to parse workload JSON in " + path.string() + ": " + exception.what());
        }

        return loadFromJson(json);
    }

    Workload WorkloadLoader::loadFromJson(const nlohmann::json& json)
    {
        const std::string name = requireStringField(json, "name", "Workload JSON");
        const std::string description = json.value("description", std::string{});

        Workload workload(name, description);

        if (!json.contains("workflows") || !json["workflows"].is_array())
        {
            throw std::runtime_error("Workload JSON: missing or invalid 'workflows' array");
        }

        for (const auto& workflowJson : json["workflows"])
        {
            Workflow workflow(requireStringField(workflowJson, "name", "Workflow"));

            if (!workflowJson.contains("hops") || !workflowJson["hops"].is_array())
            {
                throw std::runtime_error("Workflow: missing or invalid 'hops' array");
            }

            for (const auto& hopJson : workflowJson["hops"])
            {
                if (!hopJson.is_string())
                {
                    throw std::runtime_error("Workflow hop must be a string");
                }

                workflow.addHop(parseWorkflowHop(hopJson.get<std::string>()));
            }

            if (!workflowJson.contains("percentage") || !workflowJson["percentage"].is_number())
            {
                throw std::runtime_error("Workflow: missing or invalid 'percentage' field");
            }

            workload.addWorkflow(std::move(workflow), workflowJson["percentage"].get<double>());
        }

        if (!workload.isNormalized())
        {
            throw std::runtime_error(
                "Workload percentages must sum to 100, got " + std::to_string(workload.totalPercentage()));
        }

        return workload;
    }
}
