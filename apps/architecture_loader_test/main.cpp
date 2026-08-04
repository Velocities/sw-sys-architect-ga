#include <filesystem>
#include <iostream>
#include <string>

#include "architecture/ArchitectureLoader.hpp"

using namespace evoarch;

namespace
{
    std::filesystem::path defaultArchitecturePath()
    {
        return std::filesystem::path("configs") / "test_architecture.json";
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
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Failed to load architecture: " << exception.what() << "\n";
        return 1;
    }

    return 0;
}
