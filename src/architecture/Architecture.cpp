#include "architecture/Architecture.hpp"

#include <iostream>

#include <boost/range/iterator_range.hpp>

namespace evoarch
{
    Architecture::Vertex Architecture::addComponent(
        std::shared_ptr<Component> component)
    {
        Vertex vertex = boost::add_vertex(m_graph);

        m_graph[vertex].component = std::move(component);

        return vertex;
    }

    void Architecture::connect(
        Vertex from,
        Vertex to,
        double latencyMs,
        double bandwidthMbps)
    {
        auto [edge, inserted] = boost::add_edge(from, to, m_graph);

        if (!inserted)
        {
            return;
        }

        m_graph[edge].latencyMs = latencyMs;
        m_graph[edge].bandwidthMbps = bandwidthMbps;
    }

    const Architecture::Graph& Architecture::graph() const
    {
        return m_graph;
    }

    std::size_t Architecture::componentCount() const
    {
        return boost::num_vertices(m_graph);
    }

    std::size_t Architecture::connectionCount() const
    {
        return boost::num_edges(m_graph);
    }

    void Architecture::print() const
    {
        std::cout << "Architecture\n\n";

        for (auto vertex : boost::make_iterator_range(boost::vertices(m_graph)))
        {
            const auto& component = m_graph[vertex].component;

            if (component)
            {
                std::cout << "Component: " << component->id() << "\n";
            }
        }

        std::cout << "\n";

        for (auto edge : boost::make_iterator_range(boost::edges(m_graph)))
        {
            const auto from = boost::source(edge, m_graph);
            const auto to = boost::target(edge, m_graph);

            const auto& fromComponent = m_graph[from].component;
            const auto& toComponent = m_graph[to].component;

            if (!fromComponent || !toComponent)
            {
                continue;
            }

            std::cout << componentTypeName(fromComponent->type()) << "\n";
            std::cout << "  └── " << componentTypeName(toComponent->type()) << "\n\n";
        }
    }
}
