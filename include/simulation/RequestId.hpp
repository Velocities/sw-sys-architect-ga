#pragma once

#include <string>

namespace evoarch
{
    class RequestId
    {
    public:

        RequestId();

        static RequestId generate();

        const std::string& value() const;

        bool operator==(const RequestId& other) const = default;

    private:

        explicit RequestId(std::string value);

        std::string m_value;
    };
}

namespace std
{
    template<>
    struct hash<evoarch::RequestId>
    {
        std::size_t operator()(const evoarch::RequestId& requestId) const
        {
            return hash<std::string>{}(requestId.value());
        }
    };
}
