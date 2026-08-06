#include "ga/Fitness.hpp"

#include <algorithm>

namespace evoarch
{
    FitnessFunction::FitnessFunction(
        FitnessWeights weights,
        FitnessNormalization normalization)
        :
        m_weights(std::move(weights)),
        m_normalization(std::move(normalization))
    {
    }

    FitnessResult FitnessFunction::evaluate(const Metrics& metrics) const
    {
        FitnessResult result;
        result.metrics = metrics;

        const double normalizedThroughput =
            m_normalization.referenceThroughput > 0.0
                ? metrics.throughput() / m_normalization.referenceThroughput
                : 0.0;

        const double normalizedLatency =
            metrics.averageLatencyMs() > 0.0
                ? m_normalization.referenceLatencyMs / metrics.averageLatencyMs()
                : 1.0;

        const double normalizedCost =
            metrics.monthlyCost() > 0.0
                ? m_normalization.referenceCost / metrics.monthlyCost()
                : 1.0;

        result.score = (m_weights.throughput * normalizedThroughput)
                       + (m_weights.latency * normalizedLatency)
                       + (m_weights.cost * normalizedCost)
                       + (m_weights.availability * metrics.availability());

        return result;
    }

    const FitnessWeights& FitnessFunction::weights() const
    {
        return m_weights;
    }

    const FitnessNormalization& FitnessFunction::normalization() const
    {
        return m_normalization;
    }
}
