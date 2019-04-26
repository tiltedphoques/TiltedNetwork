#include "Socket.h"

Socket::Socket()
{
    m_sock = socket(AF_INET, SOCK_DGRAM, 0);
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
        new (&packet.Origin) Endpoint(pAddr->sin_addr.s_addr, pAddr->sin_port);
    }
    else
    {
        auto* pAddr = (sockaddr_in6*)&from;
        new (&packet.Origin) Endpoint((uint16_t*)&pAddr->sin6_addr, pAddr->sin6_port);
    }

    return std::move(packet);
}

bool Socket::Send(const Socket::Packet& acPacket)
{
    if (acPacket.Origin.IsIPv6())
    {
        sockaddr_in6 ipv6;
        std::memset(&ipv6, 0, sizeof(ipv6));
        ipv6.sin6_port = htons(acPacket.Origin.GetPort());
        ipv6.sin6_family = AF_INET6;
        acPacket.Origin.ToNetIPv6(ipv6.sin6_addr);
        sendto(m_sock, (const char*)acPacket.Data.GetData(), acPacket.Data.GetSize(), 0, (sockaddr*)&ipv6, sizeof(ipv6));
    }
    else
    {
        sockaddr_in ipv4;
        std::memset(&ipv4, 0, sizeof(ipv4));
        ipv4.sin_port = htons(acPacket.Origin.GetPort());
        ipv4.sin_family = AF_INET;
        acPacket.Origin.ToNetIPv4((uint32_t&)ipv4.sin_addr.s_addr);
        sendto(m_sock, (const char*)acPacket.Data.GetData(), acPacket.Data.GetSize(), 0, (sockaddr*)& ipv4, sizeof(ipv4));
    }

    return true;
}