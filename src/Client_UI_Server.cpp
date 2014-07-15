/*
   _____ _ _            _     _    _ _____    _____
  / ____| (_)          | |   | |  | |_   _|  / ____|
 | |    | |_  ___ _ __ | |_  | |  | | | |   | (___   ___ _ ____   _____ _ __
 | |    | | |/ _ \ '_ \| __| | |  | | | |    \___ \ / _ \ '__\ \ / / _ \ '__|
 | |____| | |  __/ | | | |_  | |__| |_| |_   ____) |  __/ |   \ V /  __/ |
  \_____|_|_|\___|_| |_|\__|  \____/|_____| |_____/ \___|_|    \_/ \___|_|


 */

#include "Client_UI_Server.h"

//Constructor
Client_UI_Server::Client_UI_Server(ClientMachine* clientMachine ,int port) : BasicMultithreadedServer(port)
{
	m_clientMachine = clientMachine; //set a ptr to the clientMachine
    bool isFirstSK_And_Bond_Send_req = true;
}

/* The main entry point.
 * Dispatches the welcome socket thread and returns.
*/
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
 * Receives requests from the UI client -> calls the required callbacks from
 * ClientMachine -> send the operation's result back to the UI client.
 * @param sock - a socket to the client
 * @param arg - an additional argument than can be passed by the worker thread dispatcher.
 */
int Client_UI_Server::execOnWorkerThread (SocketWrapper sock, void* arg)
{
	vector<string> parsed_request = readAndParseMessageFromSocket(sock); //receive the request

    /* for some unexplainable reason, the welcome socket can't accept message with opcode
	* OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA, which is extremely weird.
	* To overcome this, we're sending the OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA
	* opcode twice.
	*/

	//todo remove after debugging
	string temp = parsed_request[2];

	//if the ui has requested to request an SM from the server
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_REQUEST_SM_FROM_SERVER))
	{
		cout << "Requesting a state machine from the server!" << endl;
		handleRequestSM_FromServer(sock);
		cout << "DONE!" << endl;
	}

	//if the ui has requested to create SK and bond
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_CREATE_SK_AND_BOND))
	{
		cout << "Creating an SK and Bond!" << endl;
		handleRequestToCreateSK_AndBond(sock);
		cout << "DONE!" << endl;
	}

	//if the ui has requested to send the SK and bond to the CA
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA)
			&& isFirstSK_And_Bond_Send_req)
	{
		if (m_clientMachine->m_program_state == CLIENT_NEED_CA_APPROVAL ||
				m_clientMachine->m_program_state == CLIENT_OPERATIONAL)
		{
			cout << "Sending the SK and Bond to the CA!" << endl;
			handleRequestToSendSK_AndBondToCA(sock);
			cout << "DONE!" << endl;

			isFirstSK_And_Bond_Send_req=false;
		}
	}

	//if the ui has requested to send the SK and bond to the Server
//	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_SERVER)
//			&& !isFirstSK_And_Bond_Send_req)
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA)
			&& !isFirstSK_And_Bond_Send_req)
	{
		if (m_clientMachine->m_program_state == CLIENT_NEED_CA_APPROVAL)
		{
			cout << "Sending the SK and Bond to the Server!" << endl;
			handleRequestToSendSK_AndBondToServer(sock);
			cout << "DONE!" << endl;
		}
	}

	//if the ui has requested to send a message to the server
	if (!parsed_request[2].compare(OPCODE_UI_CLIENT_TO_SERVER_SEND_MSG_TO_SERVER))
	{
		if (m_clientMachine->m_program_state == CLIENT_OPERATIONAL)
		{
			cout << "Sending a message to the Server!" << endl;
			handleRequestToSendMsgToServer(sock,parsed_request[4]);
			cout << "DONE!" << endl;
		}
	}

	/*if the socket to the client UI wasn't closed by any of the handler, it means
	* that the UI clien't request can't be fulfilled in the current state (i.e. when the user request
	* to send a message to the server before receiving CA validation)
	*/
	if (sock.getSocketDescriptor() > 0)
	{
		//create a message:
		string reply_to_ui_client = createMessage(UI_SERVER,
				 UI_CLIENT,
				 OPCODE_UI_SERVER_TO_CLIENT_REQUEST_CANT_BE_HANDLED,
				 strlen(CONTENT_REQUEST_CANT_BE_FULFILLED),
				 CONTENT_REQUEST_CANT_BE_FULFILLED);

		sock.sendToSocket(reply_to_ui_client.c_str(),reply_to_ui_client.length());
		sock.closeSocket();
	}

	return 0;
}//end of webServerWorkerThread()

/*
 * Handles a UI request to send a message to the Server
 * Sends the operation's result back to the UI client
 * @param sock - socket to the UI client
 * @param msgToSend - the message to be sent to the server
 */
void Client_UI_Server::handleRequestToSendMsgToServer (SocketWrapper& sock, string& msgToSend)
{
	string reply_to_ui_client, servers_reply;
	bool isBusted=false;

	//check that the message is within the max length limits:
	if (msgToSend.length() > MAX_MSG_LENGTH)
	{
		//we need to shorten the message:
		msgToSend = msgToSend.substr(0,MAX_MSG_LENGTH);
	}

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
		case RET_VAL_TO_UI_SERVER_SERVER_DETECTED_VIRUS:
		{
			isBusted=true;
			cout << "The server has detected an attack and sent the following reply:" << endl \
					<< servers_reply << endl;
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

	if (isBusted)
		exit(0);

}//end of handleRequestToSendMsgToServer()

/*
 * Handles a UI request to send the SK and Bond to the CA
 * Sends the operation's result back to the UI client
 * @param sock - socket to the UI client
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
		case RET_VAL_TO_UI_SERVER_CA_DIDNT_RECEIVE_SK_OR_BOND:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_APPROVED_BY_CA,
					 strlen(CONTENT_CA_DIDNT_GET_SK_OR_BOND),
					 CONTENT_CA_DIDNT_GET_SK_OR_BOND);
			break;
		}
		case RET_VAL_TO_UI_SERVER_CA_DIDNT_RECEIVE_BOTH_SK_AND_BOND:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,
					 UI_CLIENT,
					 OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_APPROVED_BY_CA,
					 strlen(CONTENT_CA_DIDNT_GET_SK_AND_BOND),
					 CONTENT_CA_DIDNT_GET_SK_AND_BOND);
			break;
		}

	}//switch
	sock.sendToSocket(reply.c_str(), reply.length()); //send the reply
	sock.closeSocket();
}//end of handleRequestToSendSK_AndBondToCA()

/*
 * Handles a UI request to send the SK and Bond to the Server
 * Sends the operation's result back to the UI client
 * @param sock - socket to the UI client
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
 * @param sock - socket to the UI client
 */
void Client_UI_Server::handleRequestToCreateSK_AndBond(SocketWrapper& sock)
{
	string reply;
	int resCode = m_clientMachine->UI_Callback_CreateSK_AndBond();

	//got the operation's result. send the appropriate answer to the UI client:
	switch (resCode)
	{
		case RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATED_OK:
		{
			//we want to send the secret (the PT bond) to the UI client:
			//get the size of the PT:
			int size = m_clientMachine->m_mapper->getElementLengthInBytes(m_clientMachine->m_theSecret,true);
			unsigned char secret[size]; //allocate a buffer
			m_clientMachine->m_mapper->elementToByteArray(secret,m_clientMachine->m_theSecret, true); //translate to bytes
			/*
			 * Since the byte array is consisted out of bytes that can't be displayed on our python GUI,
			 * we're performing the following manipulation:
			 * every byte in the PT will be moved to the range between the ' ' and '~' chars (space and tilda).
			 * That range is consisted out of chars that can be displayed on screen (consult an ascii table if you don't
			 * trust us :>).
			 */

			string PTstring;
			createDisplayableBondPT_String (PTstring,(char*)secret,size);

			cout << "Generated an SK and a Bond with the following secret:\n" << PTstring << endl;
			PTstring = "The secret is:\n" + PTstring;

			//create a message:
			 reply = createMessage(UI_SERVER,UI_CLIENT,OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_CREATE_SK_AND_BOND_OK,
				PTstring.length(),PTstring);
			break;
		}
		case RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATE_FAILED:
		{
			//create a message:
			 reply = createMessage(UI_SERVER,UI_CLIENT,OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_CREATE_SK_AND_BOND_ERROR,
				strlen(CONTENT_CANT_CREATE_SK_AND_BOND),CONTENT_CANT_CREATE_SK_AND_BOND);
		}
	}//switch
	sock.sendToSocket(reply.c_str(), reply.length()); //send the reply
	sock.closeSocket();

}//end of handleRequestToCreateSK_AndBond()

/*
 * Handles a UI request to get a SM from the server.
 * Sends the operation's result back to the UI client
 * @param sock - socket to the UI client
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

