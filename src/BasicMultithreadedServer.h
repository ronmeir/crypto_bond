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

class BasicMultithreadedServer
{
private:
	WelcomeSocket* m_welcomeSock;
	int m_TCP_PortNum;

public:
	BasicMultithreadedServer(int);
	virtual ~BasicMultithreadedServer();
	virtual void run(void * (*f)(void *));
};

#endif /* BASICMULTITHREADEDSERVER_H_ */
