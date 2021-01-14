/*
   Socket.cpp

   Copyright (C) 2002-2004 Ren?Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren?Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#include <iostream>
#include <cstdio>
#include <cstring>

#include "Socket.h"
#include "crc16.h"

Socket::Socket() : s_(0)
{
	sock_init();
	// UDP: use SOCK_DGRAM instead of SOCK_STREAM
	// AF_INET - TCP/UDP family
	// IPPROTO_TCP - TCP porotocol
	s_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s_ == INVALID_SOCKET)
	{
		throw "INVALID_SOCKET";
	}

	int optval = 1;
	setsockopt(s_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	sock_nosigpipe(s_);

	refCounter_ = new int(1);
}

Socket::Socket(SOCKET s) : s_(s)
{
	sock_init();

	sock_nosigpipe(s_);

	refCounter_ = new int(1);
};

Socket::~Socket()
{
	if (!--(*refCounter_))
	{
		Close();
		sock_cleanup();
		delete refCounter_;
	}
}

Socket::Socket(const Socket &o)
{
	refCounter_ = o.refCounter_;
	(*refCounter_)++;
	s_ = o.s_;
}

Socket &Socket::operator=(Socket &o)
{
	(*o.refCounter_)++;

	refCounter_ = o.refCounter_;
	s_ = o.s_;

	return *this;
}

void Socket::Close()
{
	closesocket(s_);
}

// shutdown the send half of the connection since no more data will be sent
int Socket::ShutDown()
{
	int iResult = shutdown(s_, SD_SEND);

	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(s_);
		sock_cleanup();
		return 1;
	}
	else
		return 0;
}

std::string Socket::ReceiveBytes()
{
	std::string ret;
	char buf[DEFAULT_BUFLEN];

	for (;;)
	{
		u_long arg = 0;
		if (ioctlsocket(s_, FIONREAD, &arg) != 0)
			break;
		if (arg == 0)
			break;
		if (arg > DEFAULT_BUFLEN)
			arg = DEFAULT_BUFLEN;
		int rv = recv(s_, buf, arg, 0);
		if (rv <= 0)
			break;
		std::string t;
		t.assign(buf, rv);
		ret += t;
	}

	return ret;
}

int Socket::ReceiveData()
{

	//char buf[DEFAULT_BUFLEN];
	return recv(s_, buf_, DEFAULT_BUFLEN, 0);
}

std::string Socket::ReceiveLine()
{
	std::string ret;
	while (1)
	{
		char r;

		switch (recv(s_, &r, 1, 0))
		{
		case 0: // not connected anymore;
			return "";
		case -1:
			if (errno == EAGAIN)
			{
				return ret;
			}
			else
			{
				// not connected anymore
				return "";
			}
		}

		ret += r;
		if (r == '\n')
			return ret;
	}
}

bool Socket::SendLine(std::string s)
{
	s += '\n';
	return send(s_, s.c_str(), (int)s.length(), MSG_NOSIGNAL) >= 0;
}

bool Socket::SendBytes(const std::string &s)
{
	int iResult = send(s_, s.c_str(), (int)s.length(), MSG_NOSIGNAL);

	if ((iResult == SOCKET_ERROR) || (iResult != s.size()))
	{
		const uint8_t *p = reinterpret_cast<const uint8_t *>(s.c_str());
		std::string content("[");
		for (int i = 0; i < s.size(); ++i)
			content.append(std::to_string(p[i])).append(", ");
		content.append("]");
		//log_writef("SendBytes failed with error: %d,  content: %s, iResult: %d,\n", WSAGetLastError(), content.c_str(), iResult);
		// file_logger->debug("SendBytes failed with error: {}, content: {}, iResult: {}", WSAGetLastError(), content.c_str(), iResult);
	}

	return iResult >= 0;
}

bool Socket::SendFrame(const std::string &data)
{
    uint8_t frame_begin = 0xA5;
    std::string buf((const char*)(&frame_begin), sizeof frame_begin);
    unsigned len = data.length(); // include function code
    buf.append((const char*)&len, sizeof len);
//    buf.append((const char*)&code, sizeof code);
    buf.append(data);
    uint16_t crc = calc_crc16_len32((uint8_t*)data.data(), data.length());
    buf.append((const char*)&crc, sizeof crc);
    uint8_t frame_end = 0x5A;
    buf.append((const char*)&frame_end, sizeof frame_end);
    return SendBytes(buf);
}

bool Socket::SendBytes(const char *s)
{
	return send(s_, s, (int)strlen(s), MSG_NOSIGNAL) >= 0;
}

SocketServer::SocketServer(int port, int connections, TypeSocket type)
{
	sockaddr_in sa;

	memset(&sa, 0, sizeof(sa));

	sa.sin_family = PF_INET; //PF - protocol family == AF - address family
	sa.sin_port = htons(port);
	s_ = socket(AF_INET, SOCK_STREAM, 0);
	if (s_ == INVALID_SOCKET)
	{
		throw "INVALID_SOCKET";
	}

	if (type == NonBlockingSocket)
	{
		sock_nonblocking(s_);
	}

	int optval = 1;
	setsockopt(s_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	/* bind the socket to the internet address */
	if (bind(s_, (sockaddr *)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		closesocket(s_);
		throw "INVALID_SOCKET";
	}

	//After the socket is bound to an IP address and port on the system,
	//the server must then listen on that IP address and port for incoming connection requests.
	listen(s_, connections);
}

Socket *SocketServer::Accept()
{
	SOCKET new_sock = accept(s_, 0, 0);
	if (new_sock == INVALID_SOCKET)
	{
		int rc = WSAGetLastError();
		if (rc == WSAEWOULDBLOCK)
		{
			return 0; // non-blocking call, no request pending
		}
		else
		{
			throw "Invalid Socket";
		}
	}

	Socket *r = new Socket(new_sock);
	return r;
}

SocketClient::SocketClient(sockaddr_in &addr) : Socket()
{
	std::string error;

	if (::connect(s_, (sockaddr *)&addr, sizeof(sockaddr)))
	{
		error = strerror(WSAGetLastError());
		throw error;
	}
}

SocketClient::SocketClient(const std::string &host, int port) : Socket()
{
	std::string error;

	hostent *he;
	if ((he = gethostbyname(host.c_str())) == 0)
	{
		error = strerror(errno);
		throw error;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;	 //TCP/UDP family
	addr.sin_port = htons(port); //port
	addr.sin_addr = *((in_addr *)he->h_addr);
	memset(&(addr.sin_zero), 0, 8);

	if (::connect(s_, (sockaddr *)&addr, sizeof(sockaddr)))
	{
		error = strerror(WSAGetLastError());
		throw error;
	}
}

SocketSelect::SocketSelect(Socket const *const s1, Socket const *const s2, TypeSocket type)
{
	FD_ZERO(&fds_);
	FD_SET(const_cast<Socket *>(s1)->s_, &fds_);
	if (s2)
	{
		FD_SET(const_cast<Socket *>(s2)->s_, &fds_);
	}

	TIMEVAL tval;
	tval.tv_sec = 0;
	tval.tv_usec = 1;

	TIMEVAL *ptval;
	if (type == NonBlockingSocket)
	{
		ptval = &tval;
	}
	else
	{
		ptval = 0;
	}

	if (select(0, &fds_, (fd_set *)0, (fd_set *)0, ptval) == SOCKET_ERROR)
		throw "Error in select";
}

bool SocketSelect::Readable(Socket const *const s)
{
	if (FD_ISSET(s->s_, &fds_))
		return true;
	return false;
}
