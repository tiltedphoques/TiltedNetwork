#include "Server.h"
#include "Selector.h"

namespace TiltedPhoques
{
	Server::Server()
		: m_v4Listener(Endpoint::kIPv4)
		, m_v6Listener(Endpoint::kIPv6)
		, m_connectionManager(64)
	{

	}

	Server::~Server()
	{
	}

	bool Server::Start(uint16_t aPort)
	{
		if (m_v4Listener.Bind(aPort) == false)
		{
			return false;
		}
		return m_v6Listener.Bind(m_v4Listener.GetPort());
	}

	uint32_t Server::Update(uint64_t aElapsedMilliSeconds)
	{
		m_connectionManager.Update(aElapsedMilliSeconds);

		return Work();
	}

	uint16_t Server::GetPort() const
	{
		return m_v4Listener.GetPort();
	}

	bool Server::Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer)
	{
		if (acRemoteEndpoint.IsIPv6())
		{
			Socket::Packet packet{ acRemoteEndpoint, std::move(aBuffer) };

			m_v6Listener.Send(packet);
		}

		return false;
	}

	bool Server::ProcessPacket(Socket::Packet& aPacket)
	{
		auto pConnection = m_connectionManager.Find(aPacket.Remote);
		if (!pConnection)
		{
			Connection connection(*this, aPacket.Remote);

			m_connectionManager.Add(std::move(connection));

			return true;
		}
		else if (m_connectionManager.IsFull() == false)
		{
			// New connection


			return true;
		}

		return false;
	}

	uint32_t Server::Work()
	{
		uint32_t processedPackets = 0;

		Selector selector(m_v4Listener);
		while (selector.IsReady())
		{
			auto result = m_v4Listener.Receive();
			if (result.HasError())
			{
				// do some error handling
				continue;
			}
			else
			{
				// Route packet to a connection
				if (ProcessPacket(result.GetResult()))
					++processedPackets;
			}
		}

		Selector selectorv6(m_v6Listener);
		while (selectorv6.IsReady())
		{
			auto result = m_v6Listener.Receive();
			if (result.HasError())
			{
				// do some error handling
				continue;
			}
			else
			{
				// Route packet to a connection
				if (ProcessPacket(result.GetResult()))
					++processedPackets;
			}
		}

		return processedPackets;
	}
}