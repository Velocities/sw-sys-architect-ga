#include "ga/GeneticAlgorithm.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace evoarch
{
    namespace
    {
        // --- JSON parsing helpers ---

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

        std::size_t readGeneLimit(
            const nlohmann::json& json,
            const char* fieldName,
            std::size_t defaultValue)
        {
            if (!json.contains(fieldName))
            {
                return defaultValue;
            }

            return json[fieldName].get<std::size_t>();
        }

        GenomeConstraints parseGenomeConstraints(const nlohmann::json& json)
        {
            GenomeConstraints constraints;

            if (!json.is_object())
            {
                return constraints;
            }

            if (json.contains("api_instances") && json["api_instances"].is_object())
            {
                const auto& api = json["api_instances"];
                constraints.minApiInstances = readGeneLimit(api, "min", constraints.minApiInstances);
                constraints.maxApiInstances = readGeneLimit(api, "max", constraints.maxApiInstances);
            }

            if (json.contains("redis_instances") && json["redis_instances"].is_object())
            {
                const auto& redis = json["redis_instances"];
                constraints.minRedisInstances =
                    readGeneLimit(redis, "min", constraints.minRedisInstances);
                constraints.maxRedisInstances =
                    readGeneLimit(redis, "max", constraints.maxRedisInstances);
            }

            if (json.contains("postgres_instances") && json["postgres_instances"].is_object())
            {
                const auto& postgres = json["postgres_instances"];
                constraints.minPostgresInstances =
                    readGeneLimit(postgres, "min", constraints.minPostgresInstances);
                constraints.maxPostgresInstances =
                    readGeneLimit(postgres, "max", constraints.maxPostgresInstances);
            }

            return constraints;
        }

        FitnessWeights parseFitnessWeights(const nlohmann::json& json)
        {
            FitnessWeights weights;

            if (!json.is_object())
            {
                return weights;
            }

            weights.throughput = json.value("throughput_weight", weights.throughput);
            weights.latency = json.value("latency_weight", weights.latency);
            weights.cost = json.value("cost_weight", weights.cost);
            weights.availability = json.value("availability_weight", weights.availability);
            return weights;
        }

        FitnessNormalization parseFitnessNormalization(const nlohmann::json& json)
        {
            FitnessNormalization normalization;

            if (!json.is_object())
            {
                return normalization;
            }

            normalization.referenceThroughput =
                json.value("reference_throughput", normalization.referenceThroughput);
            normalization.referenceLatencyMs =
                json.value("reference_latency_ms", normalization.referenceLatencyMs);
            normalization.referenceCost =
                json.value("reference_cost", normalization.referenceCost);
            return normalization;
        }

        // --- Selection ---

        const Individual& selectTournamentParent(
            const Population& population,
            std::mt19937& randomEngine,
            std::size_t tournamentSize)
        {
            // Pick tournamentSize random individuals; return the fittest among them.
            std::uniform_int_distribution<std::size_t> distribution(0, population.size() - 1);

            const Individual* best = &population.individual(distribution(randomEngine));

            for (std::size_t index = 1; index < tournamentSize; ++index)
            {
                const Individual& candidate = population.individual(distribution(randomEngine));

                if (candidate.fitness > best->fitness)
                {
                    best = &candidate;
                }
            }

            return *best;
        }

        void printGenerationSummary(
            std::size_t generation,
            const Population& population)
        {
            const Individual& best = population.best();

            std::cout << "Generation " << generation << ": best fitness=" << best.fitness
                      << " (api=" << best.genome.apiInstances
                      << ", redis=" << best.genome.redisInstances
                      << ", postgres=" << best.genome.postgresInstances << ")\n";
        }
    }

    GeneticAlgorithm::GeneticAlgorithm(
        GeneticAlgorithmConfig config,
        Workload workload,
        SimulatorConfig simulatorConfig)
        :
        m_config(std::move(config)),
        m_workload(std::move(workload)),
        m_simulatorConfig(std::move(simulatorConfig))
    {
    }

    GeneticAlgorithmResult GeneticAlgorithm::run()
    {
        std::mt19937 randomEngine(m_config.seed);
        Population population(m_config.populationSize);
        population.initializeRandom(randomEngine, m_config.genomeConstraints);

        FitnessFunction fitnessFunction(
            m_config.fitnessWeights,
            m_config.fitnessNormalization);

        for (std::size_t generation = 0; generation < m_config.generations; ++generation)
        {
            // Simulate every individual and assign fitness for this generation.
            population.evaluateAll(
                m_workload,
                m_simulatorConfig,
                fitnessFunction,
                m_config.seed + static_cast<std::uint32_t>(generation * 1000U));

            printGenerationSummary(generation, population);

            if (generation + 1 >= m_config.generations)
            {
                break;
            }

            // Build the next generation from selection, crossover, and mutation.
            Population nextGeneration(m_config.populationSize);
            std::size_t nextIndex = 0;

            const Individual& currentBest = population.best();

            // Elitism — preserve the best individual unchanged.
            for (std::size_t eliteIndex = 0;
                 eliteIndex < m_config.elitismCount && nextIndex < nextGeneration.size();
                 ++eliteIndex, ++nextIndex)
            {
                nextGeneration.individual(nextIndex) = currentBest;
            }

            std::uniform_real_distribution<double> probability(0.0, 1.0);

            while (nextIndex < nextGeneration.size())
            {
                const Individual& parentA =
                    selectTournamentParent(population, randomEngine, m_config.tournamentSize);
                const Individual& parentB =
                    selectTournamentParent(population, randomEngine, m_config.tournamentSize);

                Genome childGenome = parentA.genome;

                if (probability(randomEngine) < m_config.crossoverRate)
                {
                    childGenome = Genome::crossover(parentA.genome, parentB.genome, randomEngine);
                }

                childGenome.mutate(randomEngine, m_config.genomeConstraints, m_config.mutationRate);
                childGenome.clamp(m_config.genomeConstraints);

                nextGeneration.individual(nextIndex).genome = childGenome;
                nextGeneration.individual(nextIndex).fitness = 0.0;
                nextGeneration.individual(nextIndex).metrics.reset();
                ++nextIndex;
            }

            population = std::move(nextGeneration);
        }

        // Final evaluation so returned metrics match the best genome.
        population.evaluateAll(
            m_workload,
            m_simulatorConfig,
            fitnessFunction,
            m_config.seed + static_cast<std::uint32_t>(m_config.generations * 1000U));

        GeneticAlgorithmResult result;
        result.bestIndividual = population.best();
        result.generationsCompleted = m_config.generations;
        return result;
    }

    GeneticAlgorithmConfig GeneticAlgorithmConfigLoader::loadFromFile(
        const std::filesystem::path& path)
    {
        std::ifstream input(path);

        if (!input)
        {
            throw std::runtime_error("Failed to open GA config: " + path.string());
        }

        nlohmann::json json;

        try
        {
            input >> json;
        }
        catch (const nlohmann::json::exception& exception)
        {
            throw std::runtime_error(
                "Failed to parse GA JSON in " + path.string() + ": " + exception.what());
        }

        return loadFromJson(json);
    }

    GeneticAlgorithmConfig GeneticAlgorithmConfigLoader::loadFromJson(const nlohmann::json& json)
    {
        GeneticAlgorithmConfig config;

        config.name = requireStringField(json, "name", "GA JSON");
        config.description = requireStringField(json, "description", "GA JSON");
        config.populationSize = json.value("population_size", config.populationSize);
        config.generations = json.value("generations", config.generations);
        config.mutationRate = json.value("mutation_rate", config.mutationRate);
        config.crossoverRate = json.value("crossover_rate", config.crossoverRate);
        config.tournamentSize = json.value("tournament_size", config.tournamentSize);
        config.elitismCount = json.value("elitism_count", config.elitismCount);
        config.seed = json.value("seed", config.seed);

        if (json.contains("genome"))
        {
            config.genomeConstraints = parseGenomeConstraints(json["genome"]);
        }

        if (json.contains("fitness"))
        {
            config.fitnessWeights = parseFitnessWeights(json["fitness"]);
            config.fitnessNormalization = parseFitnessNormalization(json["fitness"]);
        }

        return config;
    }
}
