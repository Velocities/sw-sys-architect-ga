#include <filesystem>
#include <iostream>
#include <string>

#include "architecture/ArchitectureLoader.hpp"
#include "simulation/Simulator.hpp"
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
    const std::filesystem::path architecturePath =
        (argc > 1) ? std::filesystem::path(argv[1]) : defaultPath("test_architecture.json");
    const std::filesystem::path workloadPath =
        (argc > 2) ? std::filesystem::path(argv[2]) : defaultPath("example_workload.json");
    const std::filesystem::path simulatorPath =
        (argc > 3) ? std::filesystem::path(argv[3]) : defaultPath("example_simulator.json");

    try
    {
        const ArchitectureDefinition architectureDefinition =
            ArchitectureLoader::loadFromFile(architecturePath);
        const Workload workload = WorkloadLoader::loadFromFile(workloadPath);
        const SimulatorConfig simulatorConfig =
            SimulatorConfigLoader::loadFromFile(simulatorPath);

        std::cout << "Running simulation\n\n";
        std::cout << "Architecture: " << architecturePath << "\n";
        std::cout << "Workload: " << workloadPath << " (" << workload.name() << ")\n";
        std::cout << "Simulator: " << simulatorPath << " (" << simulatorConfig.name << ")\n";

        if (!simulatorConfig.failureScenario.isEmpty())
        {
            std::cout << "Failure scenario: " << simulatorConfig.failureScenario.name() << " ("
                      << simulatorConfig.failureScenario.events().size() << " events)\n";
        }

        std::cout << "Duration: " << simulatorConfig.durationSeconds << " s\n";
        std::cout << "Arrival rate: " << simulatorConfig.requestsPerSecond << " req/s\n";
        std::cout << "Seed: " << simulatorConfig.seed << "\n\n";

        Simulator simulator(
            architectureDefinition.architecture,
            workload,
            simulatorConfig);

        const Metrics metrics = simulator.run();
        metrics.print();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Simulation failed: " << exception.what() << "\n";
        return 1;
    }

    return 0;
}
