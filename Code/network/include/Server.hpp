#pragma once

#include "Socket.hpp"
#include "ConnectionManager.hpp"

namespace TiltedPhoques
{
	struct Server : Connection::ICommunication
	{
		Server();
		~Server();

		TP_NOCOPYMOVE(Server);
		TP_ALLOCATOR;

		bool Start(uint16_t aPort);
		uint32_t Update(uint64_t aElapsedMilliSeconds);
		[[nodiscard]] uint16_t GetPort() const;

		bool Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer) override;

	protected:

		bool ProcessPacket(Socket::Packet& aPacket);

	private:

		uint32_t Work();

		Socket m_v4Listener;
		Socket m_v6Listener;
		ConnectionManager m_connectionManager;
	};
}