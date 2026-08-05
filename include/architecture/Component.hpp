#pragma once

#include <string>

#include "architecture/ComponentType.hpp"
#include "architecture/PerformanceModel.hpp"

namespace evoarch
{
    class Component
    {
    public:

        Component(
            std::string id,
            ComponentType type);

        Component(
            std::string id,
            ComponentType type,
            PerformanceModel model);

        const std::string& id() const;

        ComponentType type() const;

        const PerformanceModel& model() const;

    private:

        std::string m_id;

        ComponentType m_type;

        PerformanceModel m_model;
    };
}
