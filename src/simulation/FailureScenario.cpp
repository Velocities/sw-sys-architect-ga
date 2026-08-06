#include "simulation/FailureScenario.hpp"

#include <stdexcept>

namespace evoarch
{
    std::string failureEventTypeName(FailureEventType type)
    {
        switch (type)
        {
        case FailureEventType::NodeDown:
            return "node_down";
        case FailureEventType::NodeRecovery:
            return "node_recovery";
        }

        return "unknown";
    }

    FailureEventType parseFailureEventType(const std::string& name)
    {
        if (name == "node_down")
        {
            return FailureEventType::NodeDown;
        }

        if (name == "node_recovery" || name == "node_up")
        {
            return FailureEventType::NodeRecovery;
        }

        throw std::runtime_error("Unknown failure event type: " + name);
    }

    FailureScenario::FailureScenario(std::string name)
        :
        m_name(std::move(name))
    {
    }

    const std::string& FailureScenario::name() const
    {
        return m_name;
    }

    const std::vector<FailureEvent>& FailureScenario::events() const
    {
        return m_events;
    }

    bool FailureScenario::isEmpty() const
    {
        return m_events.empty();
    }

    void FailureScenario::addEvent(FailureEvent event)
    {
        m_events.push_back(std::move(event));
    }
}
