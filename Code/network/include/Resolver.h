#pragma once

#include "Network.h"
#include "Endpoint.h"

#include <string>
#include <vector>
#include <future>

class Resolver
{
public:
    class Iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = Endpoint;
        using pointer = const Endpoint*;
        using reference = const Endpoint&;
        using iterator_category = std::forward_iterator_tag;

        Iterator() : m_iterator() {}
        Iterator(std::vector<value_type>::const_iterator aIterator) : m_iterator(aIterator) {}
        Iterator& operator++() { ++m_iterator;  return *this; }
        bool operator==(const Iterator& acIterator) const { return m_iterator == acIterator.m_iterator; }
        bool operator!=(const Iterator& acIterator) const { return m_iterator != acIterator.m_iterator; }
        reference operator*() const { return *m_iterator; }
        pointer operator->() const { return m_iterator.operator->(); }

    private:
        std::vector<value_type>::const_iterator m_iterator;
    };

    Resolver(const std::string& acAddress);
    Resolver(Resolver&& aRhs) noexcept;
    Resolver(const Resolver& acRhs) = delete;
    size_t GetSize() noexcept;
    bool IsEmpty() noexcept;
    Endpoint GetEndpoint(const size_t& acIndex) noexcept;
    Endpoint operator[](const size_t& acIndex) noexcept;
    Resolver& operator=(Resolver&& aRhs) noexcept;
    Resolver& operator=(const Resolver& acRhs) = delete;
    Iterator begin() noexcept;
    Iterator end() noexcept;

protected:
    std::vector<Endpoint>& GetEndpoints() noexcept;
    void Parse(const std::string& acAddress) noexcept;
    std::vector<Endpoint> ResolveHostname(const std::string& acHostname, const uint16_t& acPort) noexcept;

private:
    std::future<std::vector<Endpoint>> m_futureEndpoints;
    std::vector<Endpoint> m_endpoints;
};
