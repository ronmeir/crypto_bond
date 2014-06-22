/*
 * Client_UI_Server.h
 * Defines a web server that is run by the client.
 * Provides the web-based UI for the project.
 */

#ifndef CLIENT_UI_SERVER_H_
#define CLIENT_UI_SERVER_H_

#include "BasicMultithreadedServer.h"
#include "ClientMachine.h"
#include "Constants.h"


class Client_UI_Server : public BasicMultithreadedServer
{
private:
	//members:
	ClientMachine* m_clientMachine;

	//methods:
	int execOnWorkerThread(SocketWrapper, void* arg);
	static void* IntermediateWelcomeSocketThreadLauncher(void *);

public:
	Client_UI_Server(ClientMachine* clientMachine, int tcp_port);
	virtual ~Client_UI_Server();
	int getPort ();
	bool run();
};

#endif /* Client_UI_Server */
