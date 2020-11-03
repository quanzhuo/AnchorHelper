/*! ------------------------------------------------------------------------------------------------------------------
* @file sock_compat.c
* @brief
*
* @attention
* Copyright 2008, 2014 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
*/
#include "SocketCompat.h"

#if defined(_WIN32)

int sock_init()
{
	WSADATA info;
	if (WSAStartup(MAKEWORD(2, 0), &info) != 0)
		return -1;
	else
		return 0;
}

void sock_cleanup()
{
	WSACleanup();
}

#elif defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
int sock_init()
{
	return 0;
}

void sock_cleanup()
{
}
#endif

int sock_nonblocking(SOCKET fd)
{
#if defined(O_NONBLOCK)
	int flags = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	unsigned long flags = 1;
	return ioctlsocket(fd, FIONBIO, &flags);
#endif
}

void sock_nosigpipe(SOCKET fd)
{
#if SO_NOSIGPIPE
	int optval = 1;
	setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof optval);
#endif
}
