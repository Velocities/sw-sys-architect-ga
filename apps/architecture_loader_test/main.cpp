#include <filesystem>
#include <iostream>
#include <string>

#include <boost/range/iterator_range.hpp>

#include "architecture/ArchitectureLoader.hpp"
#include "architecture/ComponentType.hpp"
#include "architecture/PerformanceModel.hpp"

using namespace evoarch;

namespace
{
    std::filesystem::path defaultArchitecturePath()
    {
        return std::filesystem::path("configs") / "test_architecture.json";
    }

    void printPerformanceModel(const PerformanceModel& model)
    {
        std::cout << "    Processing: " << processingDistributionName(model.processing.distribution)
                  << " (mean " << model.processing.meanMs << " ms, std dev "
                  << model.processing.stdDevMs << " ms)\n";
        std::cout << "    Max concurrent: " << model.maxConcurrentRequests << "\n";
        std::cout << "    Monthly cost: $" << model.monthlyCost << "\n";
    }
}

int main(int argc, char* argv[])
{
    const std::filesystem::path architecturePath =
        (argc > 1) ? std::filesystem::path(argv[1]) : defaultArchitecturePath();

    try
    {
        const ArchitectureDefinition definition =
            ArchitectureLoader::loadFromFile(architecturePath);

        std::cout << "Loaded architecture from: " << architecturePath << "\n\n";
        std::cout << "Name: " << definition.name << "\n";
        std::cout << "Version: " << definition.version << "\n";
        std::cout << "Description: " << definition.description << "\n\n";
        std::cout << "Components: " << definition.architecture.componentCount() << "\n";
        std::cout << "Connections: " << definition.architecture.connectionCount() << "\n\n";

        definition.architecture.print();

        std::cout << "Performance models\n\n";

        for (auto vertex :
             boost::make_iterator_range(boost::vertices(definition.architecture.graph())))
        {
            const auto& component = definition.architecture.graph()[vertex].component;

            if (!component)
            {
                continue;
            }

            std::cout << component->id() << " (" << componentTypeName(component->type()) << ")\n";
            printPerformanceModel(component->model());
            std::cout << "\n";
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Failed to load architecture: " << exception.what() << "\n";
        return 1;
    }

    return 0;
}
