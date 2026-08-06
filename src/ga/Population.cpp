#include "ga/Population.hpp"

#include <limits>
#include <stdexcept>

#include "ga/Fitness.hpp"
#include "simulation/Simulator.hpp"

namespace evoarch
{
    Population::Population(std::size_t size)
    {
        m_individuals.resize(size);
    }

    std::size_t Population::size() const
    {
        return m_individuals.size();
    }

    Individual& Population::individual(std::size_t index)
    {
        return m_individuals.at(index);
    }

    const Individual& Population::individual(std::size_t index) const
    {
        return m_individuals.at(index);
    }

    const Individual& Population::best() const
    {
        if (m_individuals.empty())
        {
            throw std::runtime_error("Cannot select best individual from an empty population");
        }

        const Individual* bestIndividual = &m_individuals.front();

        for (const Individual& candidate : m_individuals)
        {
            if (candidate.fitness > bestIndividual->fitness)
            {
                bestIndividual = &candidate;
            }
        }

        return *bestIndividual;
    }

    void Population::initializeRandom(
        std::mt19937& randomEngine,
        const GenomeConstraints& constraints)
    {
        for (Individual& member : m_individuals)
        {
            member.genome = Genome::random(randomEngine, constraints);
            member.fitness = 0.0;
            member.metrics.reset();
        }
    }

    void Population::evaluateAll(
        const Workload& workload,
        const SimulatorConfig& simulatorConfig,
        const FitnessFunction& fitnessFunction,
        std::uint32_t evaluationSeed)
    {
        for (std::size_t index = 0; index < m_individuals.size(); ++index)
        {
            Individual& member = m_individuals[index];

            SimulatorConfig runConfig = simulatorConfig;
            runConfig.seed = evaluationSeed + static_cast<std::uint32_t>(index);

            Simulator simulator(member.genome.toArchitecture(), workload, runConfig);
            const FitnessResult result = fitnessFunction.evaluate(simulator.run());

            member.metrics = result.metrics;
            member.fitness = result.score;
        }
    }
}
