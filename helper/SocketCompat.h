/*! ------------------------------------------------------------------------------------------------------------------
* @file sock_compat.h
* @brief
*
* @attention
* Copyright 2008, 2014 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
*/
#ifndef __SOCK_COMPAT__
#define __SOCK_COMPAT__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#define WSAEAGAIN WSAEWOULDBLOCK
#define setsockopt(a,b,c,d,e) setsockopt(a, b, c, (const char*)(d), e)

#if !(defined(__MINGW32__) || defined(__MINGW64__)) || defined(__MSVS2015__)
#define strdup _strdup
#endif

#if !(defined(__MINGW32__) || defined(__MINGW64__) || defined(__MSVS2015__))
#define snprintf sprintf_s
#define localtime_r(x,y) localtime_s(y,x)
#else
#define localtime_r(x,y) do { *y = *localtime(x); } while(0)
#endif

#elif defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/unistd.h>

typedef int SOCKET;
typedef struct timeval TIMEVAL;

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

#define SD_SEND SHUT_WR

#define ioctlsocket ioctl
#define closesocket close
#define WSAGetLastError() errno

#define WSAEINTR EINTR
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAECONNREFUSED ECONNREFUSED
#define WSAECONNABORTED ECONNABORTED
#define WSAEAGAIN EAGAIN
#define WSAEPROTO EPROTO

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

//MSG_NOSIGNAL and SO_NOSIGPIPE are not supported in WINSOCK

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL (0)
#endif

#ifndef SO_NOSIGPIPE
#define SO_NOSIGPIPE (0)
#endif

int sock_init();
void sock_cleanup();
int sock_nonblocking(SOCKET fd);
void sock_nosigpipe(SOCKET fd);

#ifdef __cplusplus
}
#endif

#endif
