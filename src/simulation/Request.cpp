#include "simulation/Request.hpp"

namespace evoarch
{
    Request::Request(
        RequestId id,
        Architecture::Vertex currentVertex,
        double currentTime)
        :
        m_id(std::move(id)),
        m_currentTime(currentTime),
        m_currentVertex(currentVertex)
    {
    }

    const RequestId& Request::id() const
    {
        return m_id;
    }

    double Request::currentTime() const
    {
        return m_currentTime;
    }

    double Request::totalLatency() const
    {
        return m_totalLatency;
    }

    Architecture::Vertex Request::currentVertex() const
    {
        return m_currentVertex;
    }

    void Request::setCurrentTime(double currentTime)
    {
        m_currentTime = currentTime;
    }

    void Request::setCurrentVertex(Architecture::Vertex currentVertex)
    {
        m_currentVertex = currentVertex;
    }

    void Request::addLatency(double latencyMs)
    {
        m_totalLatency += latencyMs;
    }
}
