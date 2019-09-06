#pragma once

#include "Socket.h"

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