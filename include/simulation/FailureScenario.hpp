#pragma once

#include <optional>
#include <string>
#include <vector>

namespace evoarch
{
    enum class FailureEventType
    {
        NodeDown,
        NodeRecovery
    };

    std::string failureEventTypeName(FailureEventType type);

    FailureEventType parseFailureEventType(const std::string& name);

    struct FailureEvent
    {
        FailureEventType type = FailureEventType::NodeDown;

        std::string targetComponentId;

        double atTimeMs = 0.0;

        std::optional<double> durationMs;
    };

    class FailureScenario
    {
    public:

        FailureScenario() = default;

        explicit FailureScenario(std::string name);

        const std::string& name() const;

        const std::vector<FailureEvent>& events() const;

        bool isEmpty() const;

        void addEvent(FailureEvent event);

    private:

        std::string m_name;

        std::vector<FailureEvent> m_events;
    };
}
