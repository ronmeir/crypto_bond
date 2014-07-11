/*
 __          __  _                           _____            _        _
 \ \        / / | |                         / ____|          | |      | |
  \ \  /\  / /__| | ___ ___  _ __ ___   ___| (___   ___   ___| | _____| |_
   \ \/  \/ / _ \ |/ __/ _ \| '_ ` _ \ / _ \\___ \ / _ \ / __| |/ / _ \ __|
    \  /\  /  __/ | (_| (_) | | | | | |  __/____) | (_) | (__|   <  __/ |_
     \/  \/ \___|_|\___\___/|_| |_| |_|\___|_____/ \___/ \___|_|\_\___|\__|


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
