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
	char buff[BUF_SIZE+1] = "Server echo: "; //will be used to read data from the socket
	char* ptr_to_mid_of_buffer;
	int readBytes;

	int reply_prefix_len = strlen(buff);  //get the reply prefix's length
	ptr_to_mid_of_buffer = buff+reply_prefix_len;  //set a ptr to the 1st char after the prefix
	//the data from the socket will be written from here on

	//read from socket:
    readBytes = sock.receiveFromSocket(ptr_to_mid_of_buffer,BUF_SIZE-reply_prefix_len);
    if (readBytes<1) //failed to receive data
    {
        sock.closeSocket();
        pthread_exit(NULL);
    }

    ptr_to_mid_of_buffer[readBytes]='\0';   //gonna be working with cstring

    printf("%s",buff);

    //TODO complete the actual handling
    //fucking around:
    sock.sendToSocket(buff,reply_prefix_len+readBytes);
    //finished fucking around.

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

