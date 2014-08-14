/*
 *
  ____            _        __  __       _ _   _ _   _                        _          _    _____
 |  _ \          (_)      |  \/  |     | | | (_) | | |                      | |        | |  / ____|
 | |_) | __ _ ___ _  ___  | \  / |_   _| | |_ _| |_| |__  _ __ ___  __ _  __| | ___  __| | | (___   ___ _ ____   _____ _ __
 |  _ < / _` / __| |/ __| | |\/| | | | | | __| | __| '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` |  \___ \ / _ \ '__\ \ / / _ \ '__|
 | |_) | (_| \__ \ | (__  | |  | | |_| | | |_| | |_| | | | | |  __/ (_| | (_| |  __/ (_| |  ____) |  __/ |   \ V /  __/ |
 |____/ \__,_|___/_|\___| |_|  |_|\__,_|_|\__|_|\__|_| |_|_|  \___|\__,_|\__,_|\___|\__,_| |_____/ \___|_|    \_/ \___|_|


 *
 * BasicMultithreadedServer.h
 * An abstract class that defines a basic multi-threaded server.
 * The server listens on a WelcomeSocket and dispatches worker threads to handle established connections.
 * execOnWorkerThread() must be overridden and this method is the entry point for a worker thread.
 */

#ifndef BASICMULTITHREADEDSERVER_H_
#define BASICMULTITHREADEDSERVER_H_

#include "SocketWrapper.h"
#include "WelcomeSocket.h"
#include "Constants.h"
#include "General_Funcs.h"
#include <string>
#include <pthread.h>

//used to pass multiple arguments to pthread_create
typedef struct arg_struct
{
	void* obj;
	void* arg;
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
	virtual int execOnWorkerThread(SocketWrapper, void* arg)=0;
	void runWelcomeSocket(void* argForWorkerThread);

public:
	BasicMultithreadedServer(int);
	virtual ~BasicMultithreadedServer();
	WelcomeSocket* getWelcomeSocketDescrptr();

};

#endif /* BASICMULTITHREADEDSERVER_H_ */
