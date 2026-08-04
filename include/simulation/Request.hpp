#pragma once

#include "architecture/Architecture.hpp"
#include "simulation/RequestId.hpp"

namespace evoarch
{
    class Request
    {
    public:

        Request(
            RequestId id,
            Architecture::Vertex currentVertex,
            double currentTime = 0.0);

        const RequestId& id() const;

        double currentTime() const;

        double totalLatency() const;

        Architecture::Vertex currentVertex() const;

        void setCurrentTime(double currentTime);

        void setCurrentVertex(Architecture::Vertex currentVertex);

        void addLatency(double latencyMs);

    private:

        RequestId m_id;

        double m_currentTime = 0.0;

        double m_totalLatency = 0.0;

        Architecture::Vertex m_currentVertex;
    };
}
