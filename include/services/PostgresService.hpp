#pragma once

#include "architecture/PerformanceModel.hpp"

namespace evoarch
{
    class PostgresService
    {
    public:

        static PerformanceModel defaultPerformanceModel();
    };
}
