#pragma once

#include <cstddef>
#include <vector>

#include "ga/Fitness.hpp"
#include "ga/Genome.hpp"
#include "simulation/SimulatorConfig.hpp"
#include "simulation/Workload.hpp"

namespace evoarch
{
    struct Individual
    {
        Genome genome;

        double fitness = 0.0;

        Metrics metrics;
    };

    class Population
    {
    public:

        explicit Population(std::size_t size);

        std::size_t size() const;

        Individual& individual(std::size_t index);

        const Individual& individual(std::size_t index) const;

        const Individual& best() const;

        void initializeRandom(
            std::mt19937& randomEngine,
            const GenomeConstraints& constraints);

        void evaluateAll(
            const Workload& workload,
            const SimulatorConfig& simulatorConfig,
            const FitnessFunction& fitnessFunction,
            std::uint32_t evaluationSeed);

    private:

        std::vector<Individual> m_individuals;
    };
}
