/*! ------------------------------------------------------------------------------------------------------------------
* @file shutdown.h
* @brief
*
* @attention
* Copyright 2008, 2014 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
*/
#ifndef _SHUTDOWN_H_
#define _SHUTDOWN_H_

#include "SocketCompat.h"

class Socket;

int shutdown_sleep(int ms);
int shutdown_select(SOCKET s, int timeout = -1);
int shutdown_select(Socket *s, int timeout = -1);
int shutdown_select_anc(SOCKET s, int timeout = -1);
int shutdown_select_anc(Socket *s, int timeout = -1);
void shutdown_signal();

#endif
