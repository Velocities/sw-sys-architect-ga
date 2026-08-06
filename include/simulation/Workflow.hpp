#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

#include "architecture/ComponentType.hpp"

namespace evoarch
{
    enum class WorkflowEndpoint
    {
        Client
    };

    using WorkflowHop = std::variant<WorkflowEndpoint, ComponentType>;

    std::string workflowHopLabel(const WorkflowHop& hop);

    WorkflowHop parseWorkflowHop(const std::string& hopName);

    class Workflow
    {
    public:

        explicit Workflow(std::string name);

        const std::string& name() const;

        const std::vector<WorkflowHop>& hops() const;

        void addHop(WorkflowHop hop);

        std::size_t hopCount() const;

        void print() const;

    private:

        std::string m_name;

        std::vector<WorkflowHop> m_hops;
    };
}
