/*
 * WelcomeSocket.h
 * Defines a convenient wrapper for a welcome-socket (i.e. a web server's socket that listens on port #80)
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
