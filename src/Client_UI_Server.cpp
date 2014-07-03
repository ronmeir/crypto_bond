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

	  return 0;  //silencing a warning
}//end of IntermediateWelcomeSocketThreadLauncher()

/*
 * @override
 * This method will be executed on a worker thread.
 */
int Client_UI_Server::execOnWorkerThread (SocketWrapper sock, void* arg)
{

	vector<string> parsed_request = readAndParseMessageFromSocket(sock); //receive the request

	//if the ui has requested to request an SM from the server
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_REQUEST_SM_FROM_SERVER))
	{
		handleRequestSM_FromServer(sock);
	}

	//if the ui has requested to create SK and bond
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_CREATE_SK_AND_BOND))
	{
		if (m_clientMachine->m_program_state == CLIENT_NEED_CA_APPROVAL)
			handleRequestToCreateSK_AndBond(sock);
	}

	//if the ui has requested to send the SK and bond to the CA
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA))
	{
		if (m_clientMachine->m_program_state == CLIENT_NEED_CA_APPROVAL)
			handleRequestToSendSK_AndBondToCA(sock);
	}

	//if the ui has requested to send the SK and bond to the Server
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_SERVER))
	{
		if (m_clientMachine->m_program_state == CLIENT_NEED_CA_APPROVAL)
			handleRequestToSendSK_AndBondToServer(sock);
	}

	//if the ui has requested to send a message to the server
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_MSG_TO_SERVER))
	{
		if (m_clientMachine->m_program_state == CLIENT_OPERATIONAL)
			handleRequestToSendMsgToServer(sock,parsed_request[4]);
	}

	return 0;
}//end of webServerWorkerThread()

/*
 * Handles a UI request to send a message to the Server
 * Sends the operation's result back to the UI client
 */
void Client_UI_Server::handleRequestToSendMsgToServer (SocketWrapper& sock, string& msgToSend)
{
	string reply_to_ui_client, servers_reply;
	int resCode = m_clientMachine->UI_Callback_SendMsg(servers_reply,msgToSend);//send to Server

	//got the operation's result. send the appropriate answer to the UI client:
	switch (resCode)
	{
		case RET_VAL_TO_UI_SERVER_SERVER_SENT_UNKNOWN_REPLY:
		{
			//create a message:
			reply_to_ui_client = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SERVER_REPLY,
					 strlen(CONTENT_SERVER_SENT_UNKNOWN_REPLY),
					 CONTENT_SERVER_SENT_UNKNOWN_REPLY);
			break;
		}
		case RET_VAL_TO_UI_SERVER_SERVER_REPLY_OK:
		{
			//create a message:
			reply_to_ui_client = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SERVER_REPLY,
					 servers_reply.length(),
					 servers_reply);
			break;
		}

	}//switch

	sock.sendToSocket(reply_to_ui_client.c_str(), reply_to_ui_client.length()); //send the reply
	sock.closeSocket();
}//end of handleRequestToSendMsgToServer()

/*
 * Handles a UI request to send the SK and Bond to the CA
 * Sends the operation's result back to the UI client
 */
void Client_UI_Server::handleRequestToSendSK_AndBondToCA (SocketWrapper& sock)
{
	string reply;
	int resCode = m_clientMachine->UI_Callback_SendSK_AndBond(true); //send to CA

	//got the operation's result. send the appropriate answer to the UI client:
	switch (resCode)
	{
		case RET_VAL_TO_UI_SERVER_CA_SERVICE_UNAVAILABLE:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_APPROVED_BY_CA,
					 strlen(CONTENT_CA_SERVICE_UNAVAILABLE),
					 CONTENT_CA_SERVICE_UNAVAILABLE);
			break;
		}
		case RET_VAL_TO_UI_SERVER_CA_SENT_UNKNOWN_REPLY:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_APPROVED_BY_CA,
					 strlen(CONTENT_CA_APPROVED),
					 CONTENT_CA_APPROVED);
			break;
		}
		case RET_VAL_TO_UI_SERVER_CA_DISAPPROVED_SK_AND_BOND:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_APPROVED_BY_CA,
					 strlen(CONTENT_CA_DISAPPROVED),
					 CONTENT_CA_DISAPPROVED);
			break;
		}
		case RET_VAL_TO_UI_SERVER_CA_APPROVED_SK_AND_BOND:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_APPROVED_BY_CA,
					 strlen(CONTENT_CA_APPROVED),
					 CONTENT_CA_APPROVED);
			break;
		}

	}//switch

	sock.sendToSocket(reply.c_str(), reply.length()); //send the reply
	sock.closeSocket();
}//end of handleRequestToSendSK_AndBondToCA()

/*
 * Handles a UI request to send the SK and Bond to the Server
 * Sends the operation's result back to the UI client
 */
void Client_UI_Server::handleRequestToSendSK_AndBondToServer (SocketWrapper& sock)
{
	string reply;
	int resCode = m_clientMachine->UI_Callback_SendSK_AndBond(false);//send to Server

	//got the operation's result. send the appropriate answer to the UI client:
	switch (resCode)
	{
		case RET_VAL_TO_UI_SERVER_SERVER_SENT_UNKNOWN_REPLY:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_RECEIVED_BY_SERVER,
					 strlen(CONTENT_GET_SM_ERROR),
					 CONTENT_GET_SM_ERROR);
			break;
		}
		case RET_VAL_TO_UI_SERVER_SERVER_RECEIVED_SK_AND_BOND:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_RECEIVED_BY_SERVER,
					 strlen(CONTENT_GET_SM_OK),
					 CONTENT_GET_SM_OK);
			break;
		}

	}//switch

	sock.sendToSocket(reply.c_str(), reply.length()); //send the reply
	sock.closeSocket();

}//end of handleRequestToSendSK_AndBondToServer()

/*
 * Handles a UI request to create an SK and Bond
 * Sends the operation's result back to the UI client
 */
void Client_UI_Server::handleRequestToCreateSK_AndBond(SocketWrapper& sock)
{
	string reply;
	int resCode = m_clientMachine->UI_Callback_CreateSK_AndBond();

	//got the operation's result. send the appropriate answer to the UI client:
	switch (resCode)
	{
		//at this point, we only have 1 possible result code:
		case RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATED_OK:
		{
			//we want to send the secret (the PT bond) to the UI client:

			//get the size of the PT:
			int size = m_clientMachine->m_mapper->getElementLengthInBytes(m_clientMachine->m_theSecret,true);
			unsigned char secret[size]; //allocate a buffer
			m_clientMachine->m_mapper->elementToByteArray(secret,m_clientMachine->m_theSecret, true); //translate to bytes
			string PTstring((char*)secret);   //move to a string

			//create a message:
			 reply = createMessage(UI_SERVER,UI_CLIENT,OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_CREATE_SK_AND_BOND,
				PTstring.length(),PTstring);
			break;
		}
	}//switch

	sock.sendToSocket(reply.c_str(), reply.length()); //send the reply
	sock.closeSocket();
}//end of handleRequestToCreateSK_AndBond()

/*
 * Handles a UI request to get a SM from the server.
 * Sends the operation's result back to the UI client
 */
void Client_UI_Server::handleRequestSM_FromServer(SocketWrapper& sock)
{
	string reply;
	int resCode =  m_clientMachine->UI_Callback_requestSM_FromServer();

	//got the operation's result. send the appropriate answer to the UI client:
	switch (resCode)
	{
		case RET_VAL_TO_UI_SERVER_SM_NOT_RECEIVED:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,UI_CLIENT,OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_REQUEST_SM_ERROR,
				strlen(CONTENT_GET_SM_ERROR),CONTENT_GET_SM_ERROR);
			break;
		}

		case RET_VAL_TO_UI_SERVER_SM_RECEIVED_OK:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,UI_CLIENT,OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_REQUEST_SM_OK,
				strlen(CONTENT_GET_SM_OK),CONTENT_GET_SM_OK);
			break;
		}
	}//switch

	sock.sendToSocket(reply.c_str(), reply.length()); //send the reply
	sock.closeSocket();
}//end of handleRequestSM_FromServer()

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

