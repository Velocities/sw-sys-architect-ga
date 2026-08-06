#include "simulation/Request.hpp"

namespace evoarch
{
    Request::Request(
        RequestId id,
        std::vector<WorkflowHop> hops,
        Architecture::Vertex currentVertex,
        double currentTime)
        :
        m_id(std::move(id)),
        m_hops(std::move(hops)),
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

    const std::vector<WorkflowHop>& Request::hops() const
    {
        return m_hops;
    }

    std::size_t Request::hopIndex() const
    {
        return m_hopIndex;
    }

    bool Request::isFailed() const
    {
        return m_failed;
    }

    void Request::setCurrentTime(double currentTime)
    {
        m_currentTime = currentTime;
    }

    void Request::setCurrentVertex(Architecture::Vertex currentVertex)
    {
        m_currentVertex = currentVertex;
    }

    void Request::setHopIndex(std::size_t hopIndex)
    {
        m_hopIndex = hopIndex;
    }

    void Request::setFailed(bool failed)
    {
        m_failed = failed;
    }

    void Request::addLatency(double latencyMs)
    {
        m_totalLatency += latencyMs;
    }
}
