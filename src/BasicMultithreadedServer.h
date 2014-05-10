/*
 * BasicMultithreadedServer.h
 *
 *  Created on: May 6, 2014
 *      Author: slava
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
protected:
	//members:
	WelcomeSocket* m_welcomeSock;
	int m_TCP_PortNum;

private:
	//methods:
	static void* IntermediateWorkerThreadLauncher(void *);

public:
	BasicMultithreadedServer(int);
	virtual ~BasicMultithreadedServer();
	virtual int execOnWorkerThread(SocketWrapper)=0;
	WelcomeSocket* getWelcomeSocketDescrptr();
	void runWelcomeSocket();
};

#endif /* BASICMULTITHREADEDSERVER_H_ */
