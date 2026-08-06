#pragma once

#include "simulation/Metrics.hpp"

namespace evoarch
{
    struct FitnessWeights
    {
        double throughput = 0.4;

        double latency = 0.3;

        double cost = 0.2;

        double availability = 0.1;
    };

    struct FitnessNormalization
    {
        double referenceThroughput = 20.0;

        double referenceLatencyMs = 50.0;

        double referenceCost = 400.0;
    };

    struct FitnessResult
    {
        double score = 0.0;

        Metrics metrics;
    };

    // Converts simulation Metrics into a single weighted fitness score for the GA.
    class FitnessFunction
    {
    public:

        FitnessFunction(
            FitnessWeights weights,
            FitnessNormalization normalization);

        FitnessResult evaluate(const Metrics& metrics) const;

        const FitnessWeights& weights() const;

        const FitnessNormalization& normalization() const;

    private:

        FitnessWeights m_weights;

        FitnessNormalization m_normalization;
    };
}
