#include <iostream>
#include <memory>

#include "architecture/Architecture.hpp"
#include "architecture/Component.hpp"

using namespace evoarch;

int main()
{
    Architecture architecture;

    auto lb = architecture.addComponent(
        std::make_shared<Component>("lb-1", ComponentType::LoadBalancer));
    auto api = architecture.addComponent(
        std::make_shared<Component>("api-1", ComponentType::Api));
    auto redis = architecture.addComponent(
        std::make_shared<Component>("redis-1", ComponentType::Redis));
    auto db = architecture.addComponent(
        std::make_shared<Component>("db-primary", ComponentType::Postgres));

    architecture.connect(lb, api, 1.0, 1000);
    architecture.connect(api, redis, 0.5, 1000);
    architecture.connect(redis, db, 2.5, 500);

    std::cout << "Architecture created successfully!\n\n";
    std::cout << "Components: " << architecture.componentCount() << "\n";
    std::cout << "Connections: " << architecture.connectionCount() << "\n\n";

    architecture.print();

    return 0;
}
