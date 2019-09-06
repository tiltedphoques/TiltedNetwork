#include "Network.hpp"

#include <thread>
#include <mutex>

namespace TiltedPhoques
{
	void InitializeNetwork()
	{
		static std::once_flag s_flag;
		std::call_once(s_flag, []()
			{
#ifdef _WIN32
				WSADATA wsadata;
				if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
				{
				}
#endif
			});
	}

	void ShutdownNetwork()
	{
		static std::once_flag s_flag;
		std::call_once(s_flag, []()
			{
#ifdef _WIN32
				WSACleanup();
#endif
			});
	}
}