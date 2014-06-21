/*
 * Client_UI_Server.h
 * Defines a web server that is run by the client.
 * Provides the web-based UI for the project.
 */

#ifndef CLIENT_UI_SERVER_H_
#define CLIENT_UI_SERVER_H_

#include "BasicMultithreadedServer.h"
#include "Constants.h"


class Client_UI_Server : public BasicMultithreadedServer
{
private:

	//methods:
	int execOnWorkerThread(SocketWrapper);
	static void* IntermediateWelcomeSocketThreadLauncher(void *);

public:
	Client_UI_Server(int tcp_port);
	virtual ~Client_UI_Server();
	int getPort ();
	bool run();
};

#endif /* Client_UI_Server */
