/*! ------------------------------------------------------------------------------------------------------------------
* @file shutdown.cpp
* @brief
*
* @attention
* Copyright 2008, 2014 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
*/
#include "Shutdown.h"
#include "Socket.h"

#include <pthread.h>
#include <stdlib.h>

static pthread_once_t shutdown_init_once = PTHREAD_ONCE_INIT;
static void shutdown_init();
static void shutdown_atexit();

int shutdown_select(Socket *s, int timeout)
{
	return shutdown_select(s->s_, timeout);
}

int shutdown_select_anc(Socket *s, int timeout)
{
	return shutdown_select_anc(s->s_, timeout);
}

#if defined(_WIN32)

static WSAEVENT shutdown_event;

static void shutdown_init()
{
	shutdown_event = CreateEvent(NULL, TRUE, FALSE, NULL);

	atexit(shutdown_atexit);
}

static void shutdown_atexit()
{
	WSACloseEvent(shutdown_event);
}

int shutdown_sleep(int ms)
{
	pthread_once(&shutdown_init_once, shutdown_init);

	if (ms < 0)
		ms = INFINITE;

	if (WaitForSingleObject(shutdown_event, ms) == WAIT_OBJECT_0)
		return -1;
	else
		return 0;
}

int shutdown_select(SOCKET s, int timeout)
{
	if (timeout < 0)
		timeout = INFINITE;

	pthread_once(&shutdown_init_once, shutdown_init);

	WSAEVENT events[2] = {
		shutdown_event,
		WSACreateEvent()};

	WSAEventSelect(s, events[1], FD_ACCEPT | FD_READ | FD_CLOSE);

	DWORD result = WaitForMultipleObjects(2, events, FALSE, timeout);

	//WSACloseEvent(events[1]);

	switch (result)
	{
	case WAIT_OBJECT_0:
		return -1;
	case WAIT_TIMEOUT:
		return 0;
	case WAIT_OBJECT_0 + 1:
		return 1;

	default: // Something bad happened
		return -1;
	}
}

int shutdown_select_anc(SOCKET s, int timeout)
{
	int ret = -1;

	if (timeout < 0)
		timeout = INFINITE;

	pthread_once(&shutdown_init_once, shutdown_init);

	WSAEVENT select_events[2] = {shutdown_event, WSACreateEvent()};

	WSAEventSelect(s, select_events[1], FD_READ | FD_CLOSE);

	DWORD result = WaitForMultipleObjects(2, select_events, FALSE, timeout);

	switch (result)
	{
	case WAIT_OBJECT_0:
		break;

	case WAIT_TIMEOUT:
		//printf("shutdown_select WAIT_TIMEOUT event Received. unexpected.\n");
		ret = 0;
		break;

	case WAIT_OBJECT_0 + 1:
		ret = 1;
		break;

	default: // Something bad happened
		//printf("ACHTUNG : shutdown_select Something bad happened error code : result=%d %d\n", result, GetLastError());
		break;
	}

	WSACloseEvent(select_events[1]);
	return ret;
}

void shutdown_signal()
{
	pthread_once(&shutdown_init_once, shutdown_init);

	SetEvent(shutdown_event);
}

#else

#include <poll.h>

static int shutdown_pipe[2];

static void shutdown_init()
{
	pipe(shutdown_pipe);

	atexit(shutdown_atexit);
}

static void shutdown_atexit()
{
	close(shutdown_pipe[0]);
	if (shutdown_pipe[1] != -1)
		close(shutdown_pipe[1]);
}

int shutdown_sleep(int timeout)
{
	pthread_once(&shutdown_init_once, shutdown_init);

	struct pollfd pfd[2] = {
		{.fd = shutdown_pipe[0],
		 .events = POLLIN,
		 .revents = 0},
	};

	int n = poll(pfd, 1, timeout);

	if (n == 0)
		return 0;
	else if (pfd[0].revents != 0)
		return -1;
	else
		return 1;
}

int shutdown_select(SOCKET s, int timeout)
{
	pthread_once(&shutdown_init_once, shutdown_init);

	struct pollfd pfd[2] = {
		{.fd = shutdown_pipe[0],
		 .events = POLLIN,
		 .revents = 0},
		{.fd = s,
		 .events = POLLIN,
		 .revents = 0},
	};

	int n = poll(pfd, 2, timeout);

	if (n == 0)
		return 0;
	else if (pfd[0].revents != 0)
		return -1;
	else
		return 1;
}

void shutdown_signal()
{
	pthread_once(&shutdown_init_once, shutdown_init);

	close(shutdown_pipe[1]);
	shutdown_pipe[1] = -1;
}

#ifdef __linux__
int shutdown_select_anc(SOCKET s, int timeout)
{
	return shutdown_select(s, timeout);
}
#endif

#endif
