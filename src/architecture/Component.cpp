#include "architecture/Component.hpp"

namespace evoarch
{
    Component::Component(
        std::string id,
        ComponentType type)
        :
        Component(std::move(id), type, defaultPerformanceModelForType(type))
    {
    }

    Component::Component(
        std::string id,
        ComponentType type,
        PerformanceModel model)
        :
        m_id(std::move(id)),
        m_type(type),
        m_model(std::move(model))
    {
    }

    const std::string& Component::id() const
    {
        return m_id;
    }

    ComponentType Component::type() const
    {
        return m_type;
    }

    const PerformanceModel& Component::model() const
    {
        return m_model;
    }
}
