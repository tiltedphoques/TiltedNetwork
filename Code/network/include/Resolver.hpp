#pragma once

#include "Network.hpp"
#include "Endpoint.hpp"

#include <string>
#include <vector>
#include <future>
#include "Stl.hpp"

namespace TiltedPhoques
{
    struct Resolver
    {
        struct Iterator
        {
            using difference_type = ptrdiff_t;
            using value_type = Endpoint;
            using pointer = const Endpoint*;
            using reference = const Endpoint &;
            using iterator_category = std::forward_iterator_tag;

            Iterator()
            {
            }
            Iterator(Vector<value_type>::const_iterator aIterator) : m_iterator(aIterator)
            {
            }
            Iterator& operator++()
            {
                ++m_iterator;  return *this;
            }
            bool operator==(const Iterator& acIterator) const
            {
                return m_iterator == acIterator.m_iterator;
            }
            bool operator!=(const Iterator& acIterator) const
            {
                return m_iterator != acIterator.m_iterator;
            }
            reference operator*() const
            {
                return *m_iterator;
            }
            pointer operator->() const
            {
                return m_iterator.operator->();
            }

        private:
            Vector<value_type>::const_iterator m_iterator;
        };

        Resolver(const String& acAddress);
        Resolver(Resolver&& aRhs) noexcept;
        Resolver(const Resolver& acRhs) = delete;
        [[nodiscard]] size_t GetSize() noexcept;
        [[nodiscard]] bool IsEmpty() noexcept;
        [[nodiscard]] Endpoint GetEndpoint(const size_t& acIndex) noexcept;
        [[nodiscard]] Endpoint operator[](const size_t& acIndex) noexcept;
        Resolver& operator=(Resolver&& aRhs) noexcept;
        Resolver& operator=(const Resolver& acRhs) = delete;
        Iterator begin() noexcept;
        Iterator end() noexcept;

    protected:
        [[nodiscard]] Vector<Endpoint>& GetEndpoints() noexcept;
        void Parse(String aAddress) noexcept;
        [[nodiscard]] Vector<Endpoint> ResolveHostname(const String& acHostname, const uint16_t& acPort) noexcept;

    private:
        std::future<Vector<Endpoint>> m_futureEndpoints;
        Vector<Endpoint> m_endpoints;
    };
}
