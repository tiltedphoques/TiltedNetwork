#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#elif __linux__
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <errno.h>
#else 

#endif

void InitializeNetwork();
void ShutdownNetwork();

using Socket_t = decltype(socket(AF_INET, SOCK_DGRAM, 0));