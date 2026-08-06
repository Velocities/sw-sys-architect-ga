#include <filesystem>
#include <iostream>
#include <string>

#include "ga/GeneticAlgorithm.hpp"
#include "simulation/SimulatorConfig.hpp"
#include "simulation/WorkloadLoader.hpp"

using namespace evoarch;

namespace
{
    std::filesystem::path defaultPath(const char* filename)
    {
        return std::filesystem::path("configs") / filename;
    }
}

int main(int argc, char* argv[])
{
    const std::filesystem::path gaConfigPath =
        (argc > 1) ? std::filesystem::path(argv[1]) : defaultPath("example_ga.json");
    const std::filesystem::path workloadPath =
        (argc > 2) ? std::filesystem::path(argv[2]) : defaultPath("example_workload.json");
    const std::filesystem::path simulatorPath =
        (argc > 3) ? std::filesystem::path(argv[3]) : defaultPath("example_simulator.json");

    try
    {
        const GeneticAlgorithmConfig gaConfig =
            GeneticAlgorithmConfigLoader::loadFromFile(gaConfigPath);
        const Workload workload = WorkloadLoader::loadFromFile(workloadPath);
        const SimulatorConfig simulatorConfig =
            SimulatorConfigLoader::loadFromFile(simulatorPath);

        std::cout << "Running genetic algorithm\n\n";
        std::cout << "GA config: " << gaConfigPath << " (" << gaConfig.name << ")\n";
        std::cout << "Workload: " << workloadPath << " (" << workload.name() << ")\n";
        std::cout << "Simulator: " << simulatorPath << " (" << simulatorConfig.name << ")\n";
        std::cout << "Population: " << gaConfig.populationSize << "\n";
        std::cout << "Generations: " << gaConfig.generations << "\n";
        std::cout << "Seed: " << gaConfig.seed << "\n\n";

        GeneticAlgorithm algorithm(gaConfig, workload, simulatorConfig);
        const GeneticAlgorithmResult result = algorithm.run();

        std::cout << "\nBest architecture found\n";
        std::cout << "  Fitness: " << result.bestIndividual.fitness << "\n";
        std::cout << "  API instances: " << result.bestIndividual.genome.apiInstances << "\n";
        std::cout << "  Redis instances: " << result.bestIndividual.genome.redisInstances << "\n";
        std::cout << "  Postgres instances: " << result.bestIndividual.genome.postgresInstances
                  << "\n\n";

        result.bestIndividual.metrics.print();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "GA run failed: " << exception.what() << "\n";
        return 1;
    }

    return 0;
}
