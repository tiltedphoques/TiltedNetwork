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

Outcome<Connection::Header, Connection::HeaderErrors> Connection::ProcessHeader(Buffer::Reader& aReader)
{
    Header header;

    aReader.ReadBytes((uint8_t*)&header.Signature, 2);

    if (header.Signature[0] != 'M' || header.Signature[1] != 'G')
        return kBadSignature;

    uint64_t packetType = 0;
    aReader.ReadBits(packetType, 4);
}