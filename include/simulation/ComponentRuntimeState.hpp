#pragma once

#include <cstddef>
#include <deque>
#include <optional>

#include "simulation/RequestId.hpp"

namespace evoarch
{
    class ComponentRuntimeState
    {
    public:

        void reset();

        std::size_t inFlight() const;

        std::size_t queued() const;

        bool isHealthy() const;

        void setHealthy(bool healthy);

        void incrementInFlight();

        void decrementInFlight();

        void enqueue(RequestId requestId);

        std::optional<RequestId> dequeue();

    private:

        std::size_t m_inFlight = 0;

        std::deque<RequestId> m_queue;

        bool m_isHealthy = true;
    };
}
