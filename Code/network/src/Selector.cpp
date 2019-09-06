#include "Selector.hpp"

namespace TiltedPhoques
{
	Selector::Selector(Socket& aSocket)
		: m_sock{ aSocket.m_sock }
	{
	}

	bool Selector::IsReady() const
	{
		fd_set set;
#ifdef _WIN32
		set.fd_count = 1;
		set.fd_array[0] = m_sock;
#else
		FD_ZERO(&set);
		FD_SET(m_sock, &set);
#endif

		timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = 0;

#ifdef _WIN32
		return select(set.fd_count, &set, nullptr, nullptr, &tm) == 1;
#else
		return select(m_sock + 1, &set, nullptr, nullptr, &tm) == 1;
#endif
	}
}