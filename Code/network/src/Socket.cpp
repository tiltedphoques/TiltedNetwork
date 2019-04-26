#include "Socket.h"
#include <iostream>

Socket::Socket()
{
    m_port = 0;
    m_sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (m_sock < 0)
        return; // error handling
}

Socket::~Socket()
{
#ifdef _WIN32
    closesocket(m_sock);
#else
    close(m_sock);
#endif
}

Outcome<Socket::Packet, Socket::Error> Socket::Receive()
{
    Buffer buffer(MaxPacketSize);

    sockaddr_storage from;
#ifdef _WIN32
    using socklen_t = int;
#endif
    socklen_t len = sizeof(sockaddr_storage);

    auto result = recvfrom(m_sock, (char*)buffer.GetWriteData(), MaxPacketSize, 0, (sockaddr*)&from, &len);
#ifdef _WIN32
    if (result == SOCKET_ERROR)
    {
        auto error = WSAGetLastError();

        if(error == WSAEWOULDBLOCK || error == WSAECONNRESET)
            return kDiscardError;

        return kCallFailure;
    }
#else
    if (result <= 0)
    {
        if (errno == EAGAIN)
            return kDiscardError;

        return kCallFailure;
    }
#endif

    Packet packet{ Endpoint{}, std::move(buffer) };
    if (from.ss_family == AF_INET)
    {
        auto* pAddr = (sockaddr_in*)&from;
        new (&packet.Endpoint) Endpoint(pAddr->sin_addr.s_addr, ntohs(pAddr->sin_port));
    }
    else
    {
        auto* pAddr = (sockaddr_in6*)&from;
        new (&packet.Endpoint) Endpoint((uint16_t*)&pAddr->sin6_addr, ntohs(pAddr->sin6_port));
    }

    return std::move(packet);
}

bool Socket::Send(const Socket::Packet& acPacket)
{
    if (acPacket.Endpoint.IsIPv6())
    {
        sockaddr_in6 ipv6;
        std::memset(&ipv6, 0, sizeof(ipv6));
        ipv6.sin6_port = htons(acPacket.Endpoint.GetPort());
        ipv6.sin6_family = AF_INET6;
        acPacket.Endpoint.ToNetIPv6(ipv6.sin6_addr);

        if (sendto(m_sock, (const char*)acPacket.Payload.GetData(), acPacket.Payload.GetSize(), 0, (sockaddr*)& ipv6, sizeof(ipv6)) < 0)
            return false;
    }
    else
    {
        sockaddr_in ipv4;
        std::memset(&ipv4, 0, sizeof(ipv4));
        ipv4.sin_port = htons(acPacket.Endpoint.GetPort());
        ipv4.sin_family = AF_INET;
        acPacket.Endpoint.ToNetIPv4((uint32_t&)ipv4.sin_addr.s_addr);

        if (sendto(m_sock, (const char*)acPacket.Payload.GetData(), acPacket.Payload.GetSize(), 0, (sockaddr*)&ipv4, sizeof(sockaddr_in)) < 0)
            return false; 
    }

    return true;
}

bool Socket::Bind(uint16_t aPort)
{
    sockaddr_in6 saddr;
    std::memset(&saddr, 0, sizeof(saddr));
    saddr.sin6_family = AF_INET6;
    saddr.sin6_addr = in6addr_any;
    saddr.sin6_port = htons(aPort);

    int v6only = 0;
#ifdef _WIN32
    if (setsockopt(m_sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)& v6only, sizeof(v6only)) != 0)
#else
    if (setsockopt(m_sock, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only)) != 0)
#endif
        return false;

    if (bind(m_sock, (sockaddr*)& saddr, sizeof(saddr)) < 0)
        return false;

    int len = sizeof(saddr);
    getsockname(m_sock, (sockaddr*)& saddr, &len);

    m_port = ntohs(saddr.sin6_port);

    return true;
}

uint16_t Socket::GetPort() const
{
    return m_port;
}
