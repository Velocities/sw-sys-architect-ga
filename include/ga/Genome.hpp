#pragma once

#include <cstddef>
#include <random>

#include "architecture/Architecture.hpp"

namespace evoarch
{
    struct GenomeConstraints
    {
        std::size_t minApiInstances = 1;

        std::size_t maxApiInstances = 4;

        std::size_t minRedisInstances = 1;

        std::size_t maxRedisInstances = 2;

        std::size_t minPostgresInstances = 1;

        std::size_t maxPostgresInstances = 3;
    };

    // Encodes a cache-aside architecture as instance counts for each component type.
    class Genome
    {
    public:

        std::size_t apiInstances = 1;

        std::size_t redisInstances = 1;

        std::size_t postgresInstances = 1;

        Architecture toArchitecture() const;

        void clamp(const GenomeConstraints& constraints);

        static Genome random(
            std::mt19937& randomEngine,
            const GenomeConstraints& constraints);

        static Genome crossover(
            const Genome& parentA,
            const Genome& parentB,
            std::mt19937& randomEngine);

        void mutate(
            std::mt19937& randomEngine,
            const GenomeConstraints& constraints,
            double mutationRate);
    };
}
