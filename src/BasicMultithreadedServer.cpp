/*
 * BasicMultithreadedServer.cpp
 *
 *  Created on: May 6, 2014
 *      Author: slava
 */

#include "BasicMultithreadedServer.h"

//constructor
BasicMultithreadedServer::BasicMultithreadedServer(int tcp_port)
{
	m_TCP_PortNum = tcp_port;
}

/*
 * Launches a welcome-socket that fires up worker thread for every incoming connection.
 * @param *f - ptr to the function that'll run on the worker thread
 */
void BasicMultithreadedServer::runWelcomeSocket()
{
	WelcomeSocket temp(m_TCP_PortNum);  //create a new welcome socket and start listening to the given port
	m_welcomeSock = &temp;
	SocketWrapper sa(-1);
	pthread_t threadId;

	  /* Welcome socket is now set up and bound. Wait for connection and process it. */
	while (1)
	{
		sa = m_welcomeSock->acceptNewConnection(); /* block for connection request */
		if (sa.getSocketDescriptor() < 0)
		{
			printf("Welcome socket accept failed!\n");
			continue;
		}

		//debug:

//		execOnWorkerThread(sa);
		//end of debug

		arg_struct argz;
		argz.obj=static_cast<void*>(this);
		argz.sockDescrptr=sa.getSocketDescriptor();

		if (pthread_create(&threadId, NULL,IntermediateWorkerThreadLauncher, &argz)) //create a new connection thread.
		{
			printf("Error while creating a worker thread!");
			return;
		}

		pthread_detach(threadId); //detach the thread

	} //while (1)

}//end of run()

/*
 * This method is run on a worker thread. It's used for allowing usage of this class's member methods
 * on a thread with pthread_create (which allows static or global methods only)
 *@param currentObj - a ptr to the BasicMultithreadedServer object this method is run on
 */
void* BasicMultithreadedServer::IntermediateWorkerThreadLauncher(void* argz)
{
	   arg_struct *args = ( arg_struct *)argz; //contains the 2 arguments required for this method
	  //cast to a static instance, though it actually isn't. Need to make sure this instance is alive
	  //throughout the execution of this thread:
	  BasicMultithreadedServer *thisObj = static_cast<BasicMultithreadedServer*>(args->obj);
	  thisObj->execOnWorkerThread(SocketWrapper(args->sockDescrptr));   //exec the abstract func

}//end of IntermediateWorkerThreadLauncher()

WelcomeSocket* BasicMultithreadedServer::getWelcomeSocketDescrptr()
{
	return m_welcomeSock;
}

BasicMultithreadedServer::~BasicMultithreadedServer()
{
}

