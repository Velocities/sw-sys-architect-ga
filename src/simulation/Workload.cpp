#include "simulation/Workload.hpp"

#include <cmath>
#include <iostream>
#include <utility>

namespace evoarch
{
    Workload::Workload(
        std::string name,
        std::string description)
        :
        m_name(std::move(name)),
        m_description(std::move(description))
    {
    }

    const std::string& Workload::name() const
    {
        return m_name;
    }

    const std::string& Workload::description() const
    {
        return m_description;
    }

    const std::vector<WorkflowEntry>& Workload::workflows() const
    {
        return m_workflows;
    }

    void Workload::addWorkflow(
        Workflow workflow,
        double percentage)
    {
        m_workflows.push_back(WorkflowEntry{
            .workflow = std::move(workflow),
            .percentage = percentage,
        });
    }

    std::size_t Workload::workflowCount() const
    {
        return m_workflows.size();
    }

    double Workload::totalPercentage() const
    {
        double total = 0.0;

        for (const WorkflowEntry& entry : m_workflows)
        {
            total += entry.percentage;
        }

        return total;
    }

    bool Workload::isNormalized(double tolerance) const
    {
        return std::abs(totalPercentage() - 100.0) <= tolerance;
    }

    void Workload::print() const
    {
        std::cout << m_name << "\n\n";

        if (!m_description.empty())
        {
            std::cout << m_description << "\n\n";
        }

        for (const WorkflowEntry& entry : m_workflows)
        {
            std::cout << entry.workflow.name() << "\n";
            std::cout << entry.percentage << "%\n\n";
        }

        std::cout << "Total: " << totalPercentage() << "%\n";
    }
}
