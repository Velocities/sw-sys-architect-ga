#include "simulation/Workflow.hpp"

#include <iostream>

namespace evoarch
{
    std::string workflowHopLabel(const WorkflowHop& hop)
    {
        if (const auto* endpoint = std::get_if<WorkflowEndpoint>(&hop))
        {
            switch (*endpoint)
            {
            case WorkflowEndpoint::Client:
                return "Client";
            }

            return "Unknown";
        }

        return std::string(componentTypeName(std::get<ComponentType>(hop)));
    }

    Workflow::Workflow(std::string name)
        :
        m_name(std::move(name))
    {
    }

    const std::string& Workflow::name() const
    {
        return m_name;
    }

    const std::vector<WorkflowHop>& Workflow::hops() const
    {
        return m_hops;
    }

    void Workflow::addHop(WorkflowHop hop)
    {
        m_hops.push_back(std::move(hop));
    }

    std::size_t Workflow::hopCount() const
    {
        return m_hops.size();
    }

    void Workflow::print() const
    {
        std::cout << m_name << "\n";

        for (const WorkflowHop& hop : m_hops)
        {
            std::cout << "  " << workflowHopLabel(hop) << "\n";
        }

        std::cout << "\n";
    }
}
