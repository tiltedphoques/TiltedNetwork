#pragma once

#include "Socket.hpp"

namespace TiltedPhoques
{
	struct Selector
	{
		Selector(Socket& aSocket);

		bool IsReady() const;

	private:

		Socket_t m_sock;
	};
}
