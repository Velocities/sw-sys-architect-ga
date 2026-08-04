// This file just contains the properties of the internal graph we use
#pragma once

#include <memory>

#include "architecture/Component.hpp"

namespace evoarch
{
    struct NodeProperties
    {
        std::shared_ptr<Component> component;
    };

    // At this point, the edge only models the physical connection between two components.
    // We'll almost certainly add packet loss, jitter, and maybe cost later, but this is
    // enough to get started.
    struct EdgeProperties
    {
        double latencyMs = 0.0;

        double bandwidthMbps = 0.0;
    };
}