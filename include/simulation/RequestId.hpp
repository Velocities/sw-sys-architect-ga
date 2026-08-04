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
