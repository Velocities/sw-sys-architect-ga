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

        // Normalize each metric to a comparable 0–1-ish scale using reference targets.
        const double normalizedThroughput =
            m_normalization.referenceThroughput > 0.0
                ? metrics.throughput() / m_normalization.referenceThroughput
                : 0.0;

        // Lower latency is better — invert against the reference.
        const double normalizedLatency =
            metrics.averageLatencyMs() > 0.0
                ? m_normalization.referenceLatencyMs / metrics.averageLatencyMs()
                : 1.0;

        // Lower cost is better — invert against the reference budget.
        const double normalizedCost =
            metrics.monthlyCost() > 0.0
                ? m_normalization.referenceCost / metrics.monthlyCost()
                : 1.0;

        // Weighted sum; higher score means a better architecture for the GA.
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
