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

    if (from.ss_family == AF_INET)
    {

    }
    else
    {

    }

    return Packet{ Endpoint{"127.0.0.1"}, std::move(buffer) };
}

bool Socket::Send(const Socket::Packet& acPacket)
{
   // sendto(m_sock, (const char*)acPacket.Data.GetData(), acPacket.Data.GetSize(), 0, NULL);
    return true;
}