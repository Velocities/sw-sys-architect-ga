#pragma once

#include <cstddef>
#include <vector>

#include "architecture/Architecture.hpp"
#include "simulation/RequestId.hpp"
#include "simulation/Workflow.hpp"

namespace evoarch
{
    class Request
    {
    public:

        Request(
            RequestId id,
            std::vector<WorkflowHop> hops,
            Architecture::Vertex currentVertex,
            double currentTime = 0.0);

        const RequestId& id() const;

        double currentTime() const;

        double totalLatency() const;

        Architecture::Vertex currentVertex() const;

        const std::vector<WorkflowHop>& hops() const;

        std::size_t hopIndex() const;

        bool isFailed() const;

        void setCurrentTime(double currentTime);

        void setCurrentVertex(Architecture::Vertex currentVertex);

        void setHopIndex(std::size_t hopIndex);

        void setFailed(bool failed);

        void addLatency(double latencyMs);

    private:

        RequestId m_id;

        std::vector<WorkflowHop> m_hops;

        double m_currentTime = 0.0;

        double m_totalLatency = 0.0;

        Architecture::Vertex m_currentVertex;

        std::size_t m_hopIndex = 0;

        bool m_failed = false;
    };
}
