/*
 * ClientWebServer.cpp
 *
 *  Created on: May 10, 2014
 *      Author: slava
 */

#include "Client_UI_Server.h"

Client_UI_Server::Client_UI_Server(ClientMachine* clientMachine ,int port) : BasicMultithreadedServer(port)
{
	m_clientMachine = clientMachine; //set a ptr to the clientMachine
}

bool Client_UI_Server::run()
{
	pthread_t threadId;

	if (pthread_create(&threadId, NULL,IntermediateWelcomeSocketThreadLauncher, static_cast<void*>(this))) //create a new connection thread.
	{
		printf("Error while creating a Welcome socket thread!");
		return false;
	}

	pthread_detach(threadId); //detach the thread
	return true;
}//end of run()

/*
 * Runs a welcome socket.
 */
void* Client_UI_Server::IntermediateWelcomeSocketThreadLauncher(void *obj)
{
	  //cast to a static instance, though it actually isn't. Need to make sure this instance is alive
	  //throughout the execution of this thread:
	  Client_UI_Server *thisObj = static_cast<Client_UI_Server*>(obj);
	  thisObj->runWelcomeSocket(NULL);   //start the welcome socket. runs infinitely.
}//end of IntermediateWelcomeSocketThreadLauncher()

/*
 * @override
 * This method will be executed on a worker thread.
 */
int Client_UI_Server::execOnWorkerThread (SocketWrapper sock, void* arg)
{
	char buff[BUF_SIZE+1]; //will be used to read data from the socket
	int readBytes;

    readBytes = sock.receiveFromSocket(buff,BUF_SIZE);
    if (readBytes<1) //failed to receive data
    {
        sock.closeSocket();
        pthread_exit(NULL);
    }


    buff[readBytes]='\0';   //gonna be working with cstring

    printf("%s",buff);

    //TODO complete the actual handling

    sock.closeSocket();
    return 1;
}//end of webServerWorkerThread()

int Client_UI_Server::getPort ()
{
	return m_TCP_PortNum;
}

Client_UI_Server::~Client_UI_Server()
{
	// TODO Auto-generated destructor stub
}

