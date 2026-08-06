#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

#include <nlohmann/json_fwd.hpp>

#include "ga/Fitness.hpp"
#include "ga/Genome.hpp"
#include "ga/Population.hpp"
#include "simulation/SimulatorConfig.hpp"
#include "simulation/Workload.hpp"

namespace evoarch
{
    struct GeneticAlgorithmConfig
    {
        std::string name;

        std::string description;

        std::size_t populationSize = 20;

        std::size_t generations = 10;

        double mutationRate = 0.15;

        double crossoverRate = 0.7;

        std::size_t tournamentSize = 3;

        std::size_t elitismCount = 2;

        std::uint32_t seed = 42;

        GenomeConstraints genomeConstraints;

        FitnessWeights fitnessWeights;

        FitnessNormalization fitnessNormalization;
    };

    struct GeneticAlgorithmResult
    {
        Individual bestIndividual;

        std::size_t generationsCompleted = 0;
    };

    class GeneticAlgorithm
    {
    public:

        GeneticAlgorithm(
            GeneticAlgorithmConfig config,
            Workload workload,
            SimulatorConfig simulatorConfig);

        GeneticAlgorithmResult run();

    private:

        GeneticAlgorithmConfig m_config;

        Workload m_workload;

        SimulatorConfig m_simulatorConfig;
    };

    class GeneticAlgorithmConfigLoader
    {
    public:

        static GeneticAlgorithmConfig loadFromFile(const std::filesystem::path& path);

        static GeneticAlgorithmConfig loadFromJson(const nlohmann::json& json);
    };
}
