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

	vector<string> parsed_request = readAndParseMessageFromSocket(sock); //receive the request

	//if the ui has requested to create SK and bond
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_CREATE_SK_AND_BOND))
	{

	}

	//if the ui has requested to send the SK and bond to the CA
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA))
	{

	}

	//if the ui has requested to send the SK and bond to the Server
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_SERVER))
	{

	}

	//if the ui has requested to send a message to the server
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_MSG_TO_SERVER))
	{

	}

	return 0;
}//end of webServerWorkerThread()

//vector<string> Client_UI_Server::readAndParseMessageFromUI (SocketWrapper& sock)
//{
//	char buff[BUF_SIZE];
//	int readBytes, i;
//	string temp;
//	vector<string> res;
//
//	char* ptr_to_next_empty_byte_at_buff = buff;
//
//	//read the 2 first bytes from socket:
//	for (i = 0; i < 2; i++)
//	{
//		readBytes = sock.receiveFromSocket(ptr_to_next_empty_byte_at_buff, 1);
//		ptr_to_next_empty_byte_at_buff++;
//
//		if (readBytes < 1) //failed to receive data
//		{
//			sock.closeSocket();
//			pthread_exit(NULL);
//		}
//	}
//
//	//got the first 2 bytes: OPCODE+SFSC
//
//	buff[1] = '\0';  //make it a cstring
//	temp.assign(buff);
//	res.push_back(temp); //push the 1st fields to the vector
//
//	if (  //if this is a request to create / send the SK and bond
//			!temp.compare(OPCODE_UI_CLIENT_TO_SERVER_CREATE_SK_AND_BOND) ||
//			!temp.compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA) ||
//			!temp.compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_SERVER)
//	   )
//	{
//		//no further reading from the buffer is needed. we have the opcode.
//		return res;
//	}
//
//	/* At this point, the client wishes to send a message to the server.
//	 * Further parsing of the incoming request is required:
//	 */
//	ptr_to_next_empty_byte_at_buff=buff; //reset
//
//	//now we need to read the content-length field
//    while (readBytes != 0 && readBytes != -1 && *buff != SFSC)
//    {
//    	//read the next byte:
//    	readBytes = sock.receiveFromSocket(ptr_to_next_empty_byte_at_buff,1);
//    	ptr_to_next_empty_byte_at_buff++;
//    }
//
//	if (readBytes < 1) //failed to receive data
//	{
//		sock.closeSocket();
//		pthread_exit(NULL);
//	}
//
//	//at this point, buff contains the content length
//	*(ptr_to_next_empty_byte_at_buff-1) = '\0'; //replace the SFSC with '\0'
//	temp.assign(buff);   //set to the string
//	res.push_back(temp);  //push the string to the vec
//
//	int contentLength = atoi(buff);  //get the length
//
//	contentLength = std::min(contentLength,MAX_MSG_LENGTH);  //making sure the message is smaller then the max length
//
//	ptr_to_next_empty_byte_at_buff=buff; //reset
//	i=0;
//
//	//reading the message itself:
//    while (readBytes != 0 && readBytes != -1 && i < contentLength)
//    {
//    	//read the next byte:
//    	readBytes = sock.receiveFromSocket(ptr_to_next_empty_byte_at_buff,1);
//    	ptr_to_next_empty_byte_at_buff++;
//    	i++;
//    }
//
//	if (readBytes < 1) //failed to receive data
//	{
//		sock.closeSocket();
//		pthread_exit(NULL);
//	}
//
//	buff[i]='\0';
//	temp.assign(buff);
//	res.push_back(temp);
//
//	return res;
//
//}//end of readAndParseMessageFromUI();

/*
 * Returns the tcp port number the UI server listens to.
 */
int Client_UI_Server::getPort ()
{
	return m_TCP_PortNum;
}

Client_UI_Server::~Client_UI_Server()
{
	// TODO Auto-generated destructor stub
}

