/*
   Socket.h

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

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string>
#include "SocketCompat.h"
#include "Shutdown.h"

#define DEFAULT_BUFLEN 12800

enum TypeSocket
{
	BlockingSocket,
	NonBlockingSocket
};
/**
 * @class Socket
 * @brief TCP/IP Socket Abstraction Class.
 *
 * This class provides the socket abstration of the OS.
 * <br>Implement standard Network abstraction routines implemented in this class.
 * <br>ReceiveData , ReceiveLine and SendBytes are some of the functions used in CLE.
 * @author Rene Nyffenegger
 */
class Socket
{
public:
	virtual ~Socket();
	Socket(const Socket &);
	Socket &operator=(Socket &);

	/**
		  * Receives a \\n terminated line of characters on the Socket.
		  * @return Returns a String
		  */
	std::string ReceiveLine();
	std::string ReceiveBytes();

	/**
		* Returns the data available on the socket to buf_.
		*/
	int ReceiveData();

	void Close();
	int ShutDown();
	// The parameter of SendLine is not a const reference
	// because SendLine modifes the std::string passed.
	bool SendLine(std::string);

	/**
		* Send data on the socket.
		* @param data String to Send
		*/
	bool SendBytes(const std::string &data);

	/**
		* Send data on the socket.
		* @param data Bytes to Send
		*/
	bool SendBytes(const char *data);

	/**
		* Internal Buffer to store received data.
		*/
	char buf_[DEFAULT_BUFLEN];

protected:
	friend class SocketServer;
	friend class SocketSelect;
	friend int ::shutdown_select(Socket *, int);
	friend int ::shutdown_select_anc(Socket *, int);

	Socket(SOCKET s);
	Socket();

	SOCKET s_;

	int *refCounter_;
};

/**
* @class SocketClient
* @brief TCP/IP Client Socket.
* @author Rene Nyffenegger
*/

class SocketClient : public Socket
{
public:
	/**
		* Create a socket
		* @param host Host IP Address
		* @param port Host Port
		*/
	SocketClient(const std::string &host, int port);
	SocketClient(sockaddr_in &addr);
};

/**
* @class SocketServer
* @brief TCP/IP Server Socket.
* @author Rene Nyffenegger
*/
class SocketServer : public Socket
{
public:
	/**
		* Create a Server Socket
		* @param port Listening Port
		* @param connections Number of Connections
		* @param type BlockingSocket or NonBlockingSocket
		*/
	SocketServer(int port, int connections, TypeSocket type = BlockingSocket);

	Socket *Accept();
};

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winsock/wsapiref_2tiq.asp
class SocketSelect
{
public:
	SocketSelect(Socket const *const s1, Socket const *const s2 = NULL, TypeSocket type = BlockingSocket);

	bool Readable(Socket const *const s);

private:
	fd_set fds_;
};

#endif
