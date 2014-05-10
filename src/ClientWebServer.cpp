/*
 * ClientWebServer.cpp
 *
 *  Created on: May 10, 2014
 *      Author: slava
 */

#include "ClientWebServer.h"

ClientWebServer::ClientWebServer(int port) : BasicMultithreadedServer(port)
{
}

bool ClientWebServer::run()
{
	pthread_t threadId;

	if (pthread_create(&threadId, NULL,IntermediateWelcomeSocketThreadLauncher, static_cast<void*>(this))) //create a new connection thread.
	{
		printf("Error while creating a Welcome socket thread!");
		return false;
	}

	pthread_detach(threadId); //detach the thread
	return true;
}

/*
 * This function runs a welcome socket.
 */
void* ClientWebServer::IntermediateWelcomeSocketThreadLauncher(void *obj)
{
	  //cast to a static instance, though it actually isn't. Need to make sure this instance is alive
	  //throughout the execution of this thread:
	  ClientWebServer *thisObj = static_cast<ClientWebServer*>(obj);
	  thisObj->runWelcomeSocket();   //start the welcome socket. runs infinitely.

	  pthread_exit(NULL);
}

/*
 * This method will be executed on the worker thread.
 */
int ClientWebServer::execOnWorkerThread (SocketWrapper sock)
{
	char buff[BUF_SIZE+1]; //will be used to read data from the socket
	int readBytes;

    readBytes = sock.receiveFromSocket(buff,BUF_SIZE);
    if (readBytes<1) //failed to receive data
        pthread_exit(NULL);


    buff[readBytes]='\0';   //gonna be working with cstring

    //TODO complete the actual handling


    sock.closeSocket();
    pthread_exit(NULL);
}//end of webServerWorkerThread()

int ClientWebServer::getPort ()
{
	return m_TCP_PortNum;
}

ClientWebServer::~ClientWebServer()
{
	// TODO Auto-generated destructor stub
}

