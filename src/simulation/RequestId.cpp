#include "simulation/RequestId.hpp"

#include <array>
#include <format>
#include <random>

namespace evoarch
{
    namespace
    {
        std::string formatUuidV4(std::array<std::uint8_t, 16> bytes)
        {
            bytes[6] = static_cast<std::uint8_t>((bytes[6] & 0x0F) | 0x40);
            bytes[8] = static_cast<std::uint8_t>((bytes[8] & 0x3F) | 0x80);

            return std::format(
                "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-"
                "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
                bytes[0],
                bytes[1],
                bytes[2],
                bytes[3],
                bytes[4],
                bytes[5],
                bytes[6],
                bytes[7],
                bytes[8],
                bytes[9],
                bytes[10],
                bytes[11],
                bytes[12],
                bytes[13],
                bytes[14],
                bytes[15]);
        }
    }

    RequestId::RequestId()
        :
        m_value(generate().value())
    {
    }

    RequestId::RequestId(std::string value)
        :
        m_value(std::move(value))
    {
    }

    RequestId RequestId::generate()
    {
        thread_local std::mt19937 randomEngine(std::random_device{}());

        std::array<std::uint8_t, 16> bytes{};

        for (auto& byte : bytes)
        {
            byte = static_cast<std::uint8_t>(randomEngine());
        }

        return RequestId(formatUuidV4(bytes));
    }

    const std::string& RequestId::value() const
    {
        return m_value;
    }
}
