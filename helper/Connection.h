/*! ------------------------------------------------------------------------------------------------------------------
* @file handler_connection.h
* @brief DecaWave connection handler
*
* @attention
* Copyright 2008, 2014 (c) DecaWave Ltd, Dublin, Ireland.
*
* All rights reserved.
*
*/
#ifndef _HANDLER_CONNECTION_H_
#define _HANDLER_CONNECTION_H_

#include "Thread.h"
#include "Socket.h"
#include "rtls_interface.h"
#include "crc16.h"
#include "Types.h"

#include <map>
#include <stdint.h>
#include <cstdio>

template <class Child>
class HandlerConnection : public Thread
{
public:
	HandlerConnection(Socket *s)
	{
		socket = s;
	}

	virtual ~HandlerConnection()
	{
		delete socket;
	}

	typedef size_t (Child::*handler)(const char *, size_t length);

protected:
	Socket *socket;

	void addHandler(uint8_t code, size_t minLength, handler f)
	{
		handlers.insert(std::pair<uint8_t, std::pair<size_t, handler>>(code, std::pair<size_t, handler>(minLength, f)));
	}

	virtual void Run()
	{
		unsigned int length = 0;
		std::string rxBytes;
		bool msgProcess;
		uint8_t fcode;
		unsigned int flen = 0;

		while (shutdown_select_anc(socket) != -1)
		{
			int count = socket->ReceiveData();

			if (count <= 0) // Something failed. Socket is probably closed.
			{
				break;
			}

			length += count;
			rxBytes.append(socket->buf_, count);

			/*
				* the data coming from the anchor is framed:
				*<STX><LENlsb><LENmsb><DATA:<FC><XX>.....><CRClsb><CRCmsb><ETX>
				* STX = 0x2
				* LEN is the length of data message(16 bits)
				* CRC is the 16 - bit CRC of the data bytes
				* ETX = 0x3
				* FC = is the function code (API code)
				*/
			if (length <= FRAME_HEADER_LEN) //minimum length is 7 bytes (header + 1 byte function code)
			{
				continue;
			}

			do
			{
				msgProcess = false;
				crc_err_t pckt_crc = CRC_ERROR;
				//search through the received data to find the start of frame
				while (length > FRAME_HEADER_LEN)
				{
					if ((rxBytes.c_str())[0] == 0x2) //this is the start - check if length and end match
					{
						uint8_t hi = (rxBytes.c_str())[2];
						uint8_t lo = (rxBytes.c_str())[1];

						flen = lo + ((uint16_t)hi << 8);

						if ((flen + FRAME_HEADER_LEN) > length) //we don't have all the frame yet
						{
							break;
						}

						if ((rxBytes.c_str())[flen + 5] == 0x3) //end of frame present
						{
							//received whole frame: check frame checksum here
							pckt_crc = check_crc16((uint8_t *)&(rxBytes.c_str())[3], (flen + 2));
							break;
						}
						else //no end of frame in the expected place - start byte was not real start
						{
							rxBytes.erase(0, 1);
							length -= 1;
						}
					}
					else //remove the byte and check if next is a start of frame
					{
						rxBytes.erase(0, 1);
						length -= 1;
					}
				}

				//check if we have a frame
				if (((flen + FRAME_HEADER_LEN) > length) || (length <= FRAME_HEADER_LEN))
				{
					break;
				}

				if (pckt_crc != CRC_OKAY)
				{
					//frame packet fully received but it's CRC bad
					//DBG_PRINTF(DBG_DEFAULT, "ACHTUNG ACHTUNG : incompatible software without CRC or network CRC error\n");
					// spdlog::error("ACHTUNG ACHTUNG : incompatible software without CRC or network CRC error");
					//let incompatible software proceed at the moment
				}

				fcode = (rxBytes.c_str())[FRAME_DATA_IDX]; //get function code

				typename std::map<uint8_t, std::pair<size_t, handler>>::iterator it = handlers.find(fcode);
				if (it != handlers.end()) //we have found a handler for this function code
				{
					if (flen >= it->second.first)
					{
						handler f = it->second.second;
						size_t n = (static_cast<Child *>(this)->*f)(&((rxBytes.c_str())[FRAME_DATA_IDX]), flen);
						if (n > 0)
						{
							msgProcess = true;
							rxBytes.erase(0, n + FRAME_HEADER_LEN);
							length -= (unsigned int)(n + FRAME_HEADER_LEN);
						}
						else
						{
							//Unexpected Message
							rxBytes.clear();
							length = 0;
						}
					}
				}
				else
				{
					rxBytes.clear();
					length = 0;
				}
			} while ((length > 0) && msgProcess);
		}

		socket->Close();
	}

private:
	std::map<uint8_t, std::pair<size_t, handler>> handlers;
};

class AnchorConnection : public HandlerConnection<AnchorConnection>
{
public:
	AnchorConnection(Socket *s, std::shared_ptr<helper::types::Anchor> &pa);
	virtual ~AnchorConnection() {}

protected:
	void Run();
	size_t HandleUseStaticAddr(const char *rxBytes, size_t length);
	size_t HandleSetStaticAddr(const char *rxBytes, size_t length);

private:
	std::shared_ptr<helper::types::Anchor> pa;
};

class TryConnect : public Thread
{
public:
	TryConnect() {}
	~TryConnect() {}
	void Run();
};

#endif
