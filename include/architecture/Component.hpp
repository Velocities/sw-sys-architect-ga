#pragma once

#include <string>

#include "architecture/ComponentType.hpp"

namespace evoarch
{
    class Component
    {
    public:

        Component(
            std::string id,
            ComponentType type);

        const std::string& id() const;

        ComponentType type() const;

    private:

        std::string m_id;

        ComponentType m_type;
    };
}