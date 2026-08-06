#pragma once

#include "architecture/Architecture.hpp"
#include "simulation/Metrics.hpp"
#include "simulation/SimulatorConfig.hpp"
#include "simulation/Workload.hpp"

namespace evoarch
{
    class Simulator
    {
    public:

        Simulator(
            Architecture architecture,
            Workload workload,
            SimulatorConfig config);

        Metrics run();

    private:

        Architecture m_architecture;

        Workload m_workload;

        SimulatorConfig m_config;
    };
}
