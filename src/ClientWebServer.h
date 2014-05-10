/*
 * ClientWebServer.h
 *
 *  Created on: May 10, 2014
 *      Author: slava
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
