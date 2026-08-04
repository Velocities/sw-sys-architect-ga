#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "simulation/Workflow.hpp"

namespace evoarch
{
    struct WorkflowEntry
    {
        Workflow workflow;

        double percentage = 0.0;
    };

    class Workload
    {
    public:

        Workload(
            std::string name,
            std::string description = {});

        const std::string& name() const;

        const std::string& description() const;

        const std::vector<WorkflowEntry>& workflows() const;

        void addWorkflow(
            Workflow workflow,
            double percentage);

        std::size_t workflowCount() const;

        double totalPercentage() const;

        bool isNormalized(double tolerance = 0.001) const;

        void print() const;

    private:

        std::string m_name;

        std::string m_description;

        std::vector<WorkflowEntry> m_workflows;
    };
}
