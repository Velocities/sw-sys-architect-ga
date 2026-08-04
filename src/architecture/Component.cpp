#include "architecture/Component.hpp"

namespace evoarch
{
    Component::Component(
        std::string id,
        ComponentType type)
        :
        m_id(std::move(id)),
        m_type(type)
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
}