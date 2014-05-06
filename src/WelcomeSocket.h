/*
 * WelcomeSocket.h
 *
 *  Created on: May 4, 2014
 *      Author: slava
 */
#include "Constants.h"
#include "SocketWrapper.h"

#ifndef WELCOMESOCKET_H_
#define WELCOMESOCKET_H_

class WelcomeSocket
{
private:
	//Members
	int m_socketDiscrptr;

	int initWelcomeSocket (int tcp_port);

public:
	WelcomeSocket(int);
	int getSocketDescriptor();
	void closeSocket();
	SocketWrapper acceptNewConnection();
	virtual ~WelcomeSocket();
};

#endif /* WELCOMESOCKET_H_ */
