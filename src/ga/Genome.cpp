#include "ga/Genome.hpp"

#include <algorithm>
#include <memory>
#include <random>
#include <string>

#include "architecture/Component.hpp"

namespace evoarch
{
    namespace
    {
        std::size_t randomInRange(
            std::mt19937& randomEngine,
            std::size_t minValue,
            std::size_t maxValue)
        {
            std::uniform_int_distribution<std::size_t> distribution(minValue, maxValue);
            return distribution(randomEngine);
        }

        // Convenience wrapper — applies type defaults for cost and processing time.
        Architecture::Vertex addTypedComponent(
            Architecture& architecture,
            const std::string& id,
            ComponentType type)
        {
            return architecture.addComponent(std::make_shared<Component>(id, type));
        }
    }

    Architecture Genome::toArchitecture() const
    {
        // Expand the compact genome into a full cache-aside graph topology.
        Architecture architecture;

        const Architecture::Vertex loadBalancer =
            addTypedComponent(architecture, "lb-1", ComponentType::LoadBalancer);

        std::vector<Architecture::Vertex> apiVertices;
        apiVertices.reserve(apiInstances);

        for (std::size_t index = 1; index <= apiInstances; ++index)
        {
            apiVertices.push_back(addTypedComponent(
                architecture,
                "api-" + std::to_string(index),
                ComponentType::Api));
        }

        std::vector<Architecture::Vertex> redisVertices;
        redisVertices.reserve(redisInstances);

        for (std::size_t index = 1; index <= redisInstances; ++index)
        {
            redisVertices.push_back(addTypedComponent(
                architecture,
                "redis-" + std::to_string(index),
                ComponentType::Redis));
        }

        std::vector<Architecture::Vertex> postgresVertices;
        postgresVertices.reserve(postgresInstances);

        for (std::size_t index = 0; index < postgresInstances; ++index)
        {
            // First postgres is always db-primary for failure-scenario compatibility.
            const std::string id = (index == 0) ? "db-primary" : "db-replica-" + std::to_string(index);
            postgresVertices.push_back(addTypedComponent(architecture, id, ComponentType::Postgres));
        }

        // LB fans out to every API instance.
        for (Architecture::Vertex apiVertex : apiVertices)
        {
            architecture.connect(loadBalancer, apiVertex, 1.0, 1000.0);
        }

        // Full mesh between APIs and Redis nodes (cache layer).
        for (Architecture::Vertex apiVertex : apiVertices)
        {
            for (Architecture::Vertex redisVertex : redisVertices)
            {
                architecture.connect(apiVertex, redisVertex, 0.5, 1000.0);
            }
        }

        // Every Redis connects to every Postgres for failover routing.
        for (Architecture::Vertex redisVertex : redisVertices)
        {
            for (Architecture::Vertex postgresVertex : postgresVertices)
            {
                architecture.connect(redisVertex, postgresVertex, 2.5, 500.0);
            }
        }

        return architecture;
    }

    void Genome::clamp(const GenomeConstraints& constraints)
    {
        // Keep gene values within the search bounds after crossover or mutation.
        apiInstances = std::clamp(
            apiInstances,
            constraints.minApiInstances,
            constraints.maxApiInstances);
        redisInstances = std::clamp(
            redisInstances,
            constraints.minRedisInstances,
            constraints.maxRedisInstances);
        postgresInstances = std::clamp(
            postgresInstances,
            constraints.minPostgresInstances,
            constraints.maxPostgresInstances);
    }

    Genome Genome::random(
        std::mt19937& randomEngine,
        const GenomeConstraints& constraints)
    {
        // Uniform random genome for initial population seeding.
        Genome genome;
        genome.apiInstances =
            randomInRange(randomEngine, constraints.minApiInstances, constraints.maxApiInstances);
        genome.redisInstances =
            randomInRange(randomEngine, constraints.minRedisInstances, constraints.maxRedisInstances);
        genome.postgresInstances = randomInRange(
            randomEngine,
            constraints.minPostgresInstances,
            constraints.maxPostgresInstances);
        return genome;
    }

    Genome Genome::crossover(
        const Genome& parentA,
        const Genome& parentB,
        std::mt19937& randomEngine)
    {
        // Uniform crossover — each gene independently inherited from one parent.
        std::uniform_int_distribution<int> coinFlip(0, 1);

        Genome child;
        child.apiInstances = coinFlip(randomEngine) ? parentA.apiInstances : parentB.apiInstances;
        child.redisInstances = coinFlip(randomEngine) ? parentA.redisInstances : parentB.redisInstances;
        child.postgresInstances =
            coinFlip(randomEngine) ? parentA.postgresInstances : parentB.postgresInstances;
        return child;
    }

    void Genome::mutate(
        std::mt19937& randomEngine,
        const GenomeConstraints& constraints,
        double mutationRate)
    {
        // Each gene independently has a chance to shift by ±1 instance count.
        std::uniform_real_distribution<double> probability(0.0, 1.0);
        std::uniform_int_distribution<int> deltaDistribution(-1, 1);

        if (probability(randomEngine) < mutationRate)
        {
            const int delta = deltaDistribution(randomEngine);

            if (static_cast<int>(apiInstances) + delta
                >= static_cast<int>(constraints.minApiInstances))
            {
                apiInstances = static_cast<std::size_t>(
                    static_cast<int>(apiInstances) + delta);
            }
        }

        if (probability(randomEngine) < mutationRate)
        {
            const int delta = deltaDistribution(randomEngine);

            if (static_cast<int>(redisInstances) + delta
                >= static_cast<int>(constraints.minRedisInstances))
            {
                redisInstances = static_cast<std::size_t>(
                    static_cast<int>(redisInstances) + delta);
            }
        }

        if (probability(randomEngine) < mutationRate)
        {
            const int delta = deltaDistribution(randomEngine);

            if (static_cast<int>(postgresInstances) + delta
                >= static_cast<int>(constraints.minPostgresInstances))
            {
                postgresInstances = static_cast<std::size_t>(
                    static_cast<int>(postgresInstances) + delta);
            }
        }

        clamp(constraints);
    }
}
