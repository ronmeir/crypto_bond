/*
 * BasicMultithreadedServer.h
 * An abstract class that defines a basic multi-threaded server.
 * The server listens on a WelcomeSocket and dispatches worker threads to handle established connections.
 * The worker-thread functionality should override execOnWorkerThread().
 */

#ifndef BASICMULTITHREADEDSERVER_H_
#define BASICMULTITHREADEDSERVER_H_

#include "SocketWrapper.h"
#include "WelcomeSocket.h"
#include "Constants.h"
#include <string>
#include <pthread.h>

//used to pass multiple arguments to pthread_create
typedef struct arg_struct
{
	void* obj;
	int sockDescrptr;
}arg_struct;

class BasicMultithreadedServer
{
private:
	//methods:
	static void* IntermediateWorkerThreadLauncher(void *);

protected:
	//members:
	WelcomeSocket* m_welcomeSock;
	int m_TCP_PortNum;

	//methods:
	virtual int execOnWorkerThread(SocketWrapper)=0;
	void runWelcomeSocket();

public:
	BasicMultithreadedServer(int);
	virtual ~BasicMultithreadedServer();
	WelcomeSocket* getWelcomeSocketDescrptr();

};

#endif /* BASICMULTITHREADEDSERVER_H_ */
