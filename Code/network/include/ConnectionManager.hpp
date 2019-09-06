#pragma once

#include "Socket.hpp"
#include "Connection.hpp"
#include <unordered_map>

namespace TiltedPhoques
{
	struct ConnectionManager : public AllocatorCompatible
	{
		ConnectionManager(size_t aMaxConnections);

		Connection* Find(const Endpoint& acEndpoint);
		const Connection* Find(const Endpoint& acEndpoint) const;

		void Add(Connection aConnection);

		bool IsFull() const;

		void Update(uint64_t aElapsedMilliSeconds);

	private:

		std::unordered_map<Endpoint, Connection> m_connections;
		size_t m_maxConnections;
	};
}