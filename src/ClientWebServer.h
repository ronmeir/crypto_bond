/*
 * ClientWebServer.h
 * Defines a web server that is run by the client.
 * Provides the web-based UI for the project.
 */

#ifndef CLIENTWEBSERVER_H_
#define CLIENTWEBSERVER_H_

#include "BasicMultithreadedServer.h"
#include "Constants.h"


class ClientWebServer : public BasicMultithreadedServer
{
private:

	//methods:
	int execOnWorkerThread(SocketWrapper);
	static void* IntermediateWelcomeSocketThreadLauncher(void *);

public:
	ClientWebServer(int tcp_port);
	virtual ~ClientWebServer();
	int getPort ();
	bool run();
};

#endif /* CLIENTWEBSERVER_H_ */
