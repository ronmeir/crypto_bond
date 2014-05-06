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
void BasicMultithreadedServer::run(void* (*f)(void *))
{
	WelcomeSocket temp(m_TCP_PortNum);  //create a new welcome socket and start listening in the given port
	m_welcomeSock = &temp;
	SocketWrapper sa(0);
	pthread_t threadId;

	  /* Socket is now set up and bound. Wait for connection and process it. */
	while (1)
	{
		sa = m_welcomeSock->acceptNewConnection(); /* block for connection request */
		if (sa.getSocketDescriptor() < 0)
		{
			printf("Welcome socket accept failed!\n");
			continue;
		}

		if (pthread_create(&threadId, NULL,f, &sa)) //create a new connection thread.
		{
			printf("Error while creating a worker thread!");
			return;
		}

		pthread_detach(threadId); //detach the thread

	} //while (1)

}//end of run()

BasicMultithreadedServer::~BasicMultithreadedServer()
{
}

