#include "simulation/ComponentRuntimeState.hpp"

namespace evoarch
{
    void ComponentRuntimeState::reset()
    {
        m_inFlight = 0;
        m_queue.clear();
        m_isHealthy = true;
    }

    std::size_t ComponentRuntimeState::inFlight() const
    {
        return m_inFlight;
    }

    std::size_t ComponentRuntimeState::queued() const
    {
        return m_queue.size();
    }

    bool ComponentRuntimeState::isHealthy() const
    {
        return m_isHealthy;
    }

    void ComponentRuntimeState::setHealthy(bool healthy)
    {
        m_isHealthy = healthy;
    }

    void ComponentRuntimeState::incrementInFlight()
    {
        ++m_inFlight;
    }

    void ComponentRuntimeState::decrementInFlight()
    {
        if (m_inFlight > 0)
        {
            --m_inFlight;
        }
    }

    void ComponentRuntimeState::enqueue(RequestId requestId)
    {
        m_queue.push_back(std::move(requestId));
    }

    std::optional<RequestId> ComponentRuntimeState::dequeue()
    {
        if (m_queue.empty())
        {
            return std::nullopt;
        }

        RequestId requestId = std::move(m_queue.front());
        m_queue.pop_front();
        return requestId;
    }
}
