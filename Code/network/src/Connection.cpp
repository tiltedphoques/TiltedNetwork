#include "Connection.h"


Connection::Connection()
    : m_state{kNone}
{

}

bool Connection::ProcessPacket(Buffer* apBuffer)
{
    Buffer::Reader reader(apBuffer);
    
    


    return false;
}

bool Connection::ProcessNegociation(Buffer* apBuffer)
{
    Buffer::Reader reader(apBuffer);

    uint8_t signature[2];
    reader.ReadBytes(signature, 2);

    return IsNegotiating() || IsConnected();
}

bool Connection::IsNegotiating() const
{
    return m_state == kNegociating;
}

bool Connection::IsConnected() const
{
    return m_state == kConnected;
}

void Connection::ProcessHeader(Buffer::Reader& aReader)
{
    uint8_t signature[2];
    aReader.ReadBytes(signature, 2);

    if (signature[0] != 'M' || signature[1] != 'G')
        return;

    uint64_t packetType = 0;
    aReader.ReadBits(packetType, 4);
}