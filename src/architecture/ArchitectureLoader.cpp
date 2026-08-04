#include "architecture/ArchitectureLoader.hpp"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "architecture/Component.hpp"
#include "architecture/ComponentType.hpp"

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

        double requireNumberField(
            const nlohmann::json& json,
            const char* fieldName,
            const char* context)
        {
            if (!json.contains(fieldName) || !json[fieldName].is_number())
            {
                throw std::runtime_error(
                    std::string(context) + ": missing or invalid number field '" + fieldName + "'");
            }

            return json[fieldName].get<double>();
        }
    }

    ComponentType parseComponentType(const std::string& typeName)
    {
        if (typeName == "Api" || typeName == "API" || typeName == "api")
        {
            return ComponentType::Api;
        }

        if (typeName == "Redis" || typeName == "redis")
        {
            return ComponentType::Redis;
        }

        if (typeName == "Postgres" || typeName == "PostgreSQL" || typeName == "postgres")
        {
            return ComponentType::Postgres;
        }

        if (typeName == "LoadBalancer" || typeName == "load_balancer" || typeName == "loadbalancer")
        {
            return ComponentType::LoadBalancer;
        }

        throw std::runtime_error("Unknown component type: " + typeName);
    }

    ArchitectureDefinition ArchitectureLoader::loadFromFile(const std::filesystem::path& path)
    {
        std::ifstream input(path);

        if (!input)
        {
            throw std::runtime_error("Failed to open architecture file: " + path.string());
        }

        nlohmann::json json;

        try
        {
            input >> json;
        }
        catch (const nlohmann::json::exception& exception)
        {
            throw std::runtime_error(
                "Failed to parse architecture JSON in " + path.string() + ": "
                + exception.what());
        }

        return loadFromJson(json);
    }

    ArchitectureDefinition ArchitectureLoader::loadFromJson(const nlohmann::json& json)
    {
        ArchitectureDefinition definition;

        definition.name = requireStringField(json, "name", "Architecture JSON");
        definition.version = requireStringField(json, "version", "Architecture JSON");
        definition.description = requireStringField(json, "description", "Architecture JSON");

        if (!json.contains("nodes") || !json["nodes"].is_array())
        {
            throw std::runtime_error("Architecture JSON: missing or invalid 'nodes' array");
        }

        if (!json.contains("edges") || !json["edges"].is_array())
        {
            throw std::runtime_error("Architecture JSON: missing or invalid 'edges' array");
        }

        std::unordered_map<std::string, Architecture::Vertex> verticesById;

        for (const auto& node : json["nodes"])
        {
            const std::string nodeId = requireStringField(node, "id", "Node");
            const std::string nodeType = requireStringField(node, "type", "Node");

            if (verticesById.contains(nodeId))
            {
                throw std::runtime_error("Duplicate node id: " + nodeId);
            }

            const auto vertex = definition.architecture.addComponent(
                std::make_shared<Component>(nodeId, parseComponentType(nodeType)));

            verticesById.emplace(nodeId, vertex);
        }

        for (const auto& edge : json["edges"])
        {
            const std::string fromId = requireStringField(edge, "from", "Edge");
            const std::string toId = requireStringField(edge, "to", "Edge");
            const double latencyMs = requireNumberField(edge, "latency_ms", "Edge");
            const double bandwidthMbps = requireNumberField(edge, "bandwidth_mbps", "Edge");

            const auto fromIt = verticesById.find(fromId);
            if (fromIt == verticesById.end())
            {
                throw std::runtime_error("Edge references unknown source node: " + fromId);
            }

            const auto toIt = verticesById.find(toId);
            if (toIt == verticesById.end())
            {
                throw std::runtime_error("Edge references unknown target node: " + toId);
            }

            definition.architecture.connect(
                fromIt->second,
                toIt->second,
                latencyMs,
                bandwidthMbps);
        }

        return definition;
    }
}
