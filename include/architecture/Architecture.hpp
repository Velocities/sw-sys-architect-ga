#pragma once

#include <cstddef>
#include <memory>

#include <boost/graph/adjacency_list.hpp>

#include "architecture/GraphProperties.hpp"

namespace evoarch
{
    class Architecture
    {
    public:

        using Graph = boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::directedS,
            NodeProperties,
            EdgeProperties>;

        using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
        using Edge = boost::graph_traits<Graph>::edge_descriptor;

    public:

        Vertex addComponent(std::shared_ptr<Component> component);

        void connect(
            Vertex from,
            Vertex to,
            double latencyMs,
            double bandwidthMbps);

        const Graph& graph() const;

        std::size_t componentCount() const;

        std::size_t connectionCount() const;

        void print() const;

    private:

        Graph m_graph;
    };
}