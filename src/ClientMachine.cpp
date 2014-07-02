/*
 * UserMachine.cpp
 * Describes the entire client machine.
 */

#include "ClientMachine.h"


using namespace std;

ClientMachine::ClientMachine(const string userID,const string ServerIP,const string CA_IP)
{
	//init:
	m_ID.assign(userID);
	m_ServerIP.assign(ServerIP);
	m_CA_IP.assign(CA_IP);
	m_mapper = new BilinearMappingHandler(PARAM_FILE_PATH); //we require a mapper to deserialize the SM string
	m_EncHandler = NULL;   //can't be initialized yet because it requires an SM
	m_SM = NULL;
	m_SK = NULL;
	m_MSK = NULL;
	m_Bond = NULL;
	m_serializer = new ObjectSerializer (*m_mapper);
	m_UI_Server = new Client_UI_Server (this,CLIENT_UI_SERVER_TCP_PORT_NUM);
	m_program_state = NEED_STATE_MACHINE;  //init the program's state

}//end of constructor

/**
 * Sends a single message to the servers and waits for a reply.
 * @param msg- the msg to send
 * @param servers_reply - the server's reply will be save here
 * @return - the result OPCODE
 */
int ClientMachine::UI_Callback_SendMsg(string servers_reply, string msg)
{
	string msgToSend = createMessage(m_ID, SERVER_NAME,OPCODE_CLIENT_TO_SERVER_SEND_MSG,
			msg.size(), msg);

	SocketWrapper sock_to_server(m_ServerIP,SERVER_AND_CA_TCP_PORT_NUM); //open a sock
	sock_to_server.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the request

	vector<string> parsed_reply = readAndParseMessageFromSocket(
			sock_to_server); //receive the reply

	sock_to_server.closeSocket(); //close the socket

	//if the opcode doesn't match
	if (parsed_reply[2].compare(OPCODE_SERVER_TO_CLIENT_ECHO_MSG))
	{
		servers_reply = "Unknown server response!";
		return RET_VAL_TO_UI_SERVER_SERVER_SENT_UNKNOWN_REPLY;
	}

	servers_reply = parsed_reply[4];   //set the server's message content
	return RET_VAL_TO_UI_SERVER_SERVER_REPLY_OK;

}//end of UI_Callback_SendMsg()

/**
 * Initializes all required parameters and creates an SK and Bond.
 */
int ClientMachine::UI_Callback_CreateSK_AndBond()
{
	if (m_program_state == NEED_STATE_MACHINE) //if we have no SM yet
		return RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATE_FAILED;

	if (m_EncHandler!=NULL) //if an enc_handler already exists
		delete(m_EncHandler);

	m_EncHandler = new EncryptionHandler(PARAM_FILE_PATH,m_SM,true);

	m_MSK = m_EncHandler->setup(); //gen. master key
	m_SK = m_EncHandler->keyGen();	//gen. secret key

	//map the bond to some random element in G1
	m_EncHandler->mapStringToElementFromGT(m_theSecret, "BOND STRING");

	m_Bond = new EncryptionHandler::CT(m_EncHandler->getBilinearMappingHandler(),
			MAX_MSG_LENGTH, true);  //creating a new empty CT
	m_EncHandler->createPartialEncryption(*m_Bond,m_virus, m_theSecret); //generate a partial CT

	m_serializer->setSecretKey(*m_SK,*m_SM);  //set the SK into the serializer
	m_serializer->setBond(*m_Bond);			  //set the Bond into the serializer

	return RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATED_OK;

}//end of UI_Callback_CreateSK_AndBond()

/*
 * Sends the SK and Bond to the CA/Serve, waits for a reply on each one before sending the next.
 * Message order:
 * 1. Sends SK message and waits for an ACK.
 * 2. Sends Bond message and waits for an ACK.
 * 4. If the dst is the CA, sends the bond in plaintext
 * 3. If the dst is the CA, sends a Validation-Request message and waits for a reply.
 */
int ClientMachine::UI_Callback_SendSK_AndBond(bool isSendToCA)
{
	string msgToSend, content;

	if(isSendToCA)
		m_serializer->setBondInPlainText(m_theSecret,true); //set the PT in the bond
	else
		m_serializer->clearBondInPlainText();			    //make sure there's no PT in the bond

	for (int i = 0; i < 2; i++) //repeat twice, once for the SK and once for the Bond
	{
		i==0?
		m_serializer->getSerializedSecretKeyString(content): //get the serialized SK
		m_serializer->getSerializedBondString(content);      //get the serialized Bond

		//create a message containing the SK/Bond:

		if (isSendToCA)  //if the message is to be sent to the CA
		{
		msgToSend = createMessage(m_ID, CA_NAME,
				i==0? OPCODE_CLIENT_TO_CA_SEND_SK : OPCODE_CLIENT_TO_CA_SEND_ENCRYPTED_BOND,
				content.size(), content);
		}
		else //the message is to be sent to the server
		{
			msgToSend = createMessage(m_ID, SERVER_NAME,
					i==0? OPCODE_CLIENT_TO_SERVER_SEND_SK : OPCODE_CLIENT_TO_SERVER_SEND_BOND,
					content.size(), content);
		}

		SocketWrapper sock_to_server(isSendToCA ? m_CA_IP : m_ServerIP,SERVER_AND_CA_TCP_PORT_NUM); //open a sock
		sock_to_server.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the request

		vector<string> parsed_reply = readAndParseMessageFromSocket(
				sock_to_server); //receive the reply

		sock_to_server.closeSocket(); //close the socket
		//GOT A REPLY.
		//checking the reply:

		if (isSendToCA) //if the reply came from the CA
		{
			//if the opcode or content don't match
			if (parsed_reply[2].compare( i == 0 ? OPCODE_CA_TO_CLIENT_ACK_SK : OPCODE_CA_TO_CLIENT_ACK_ENCRYPTED_BOND)
					|| parsed_reply[4].compare(CONTENT_ACK))
				return RET_VAL_TO_UI_SERVER_CA_SENT_UNKNOWN_REPLY;

		}
		else  //the reply came from the server
		{
			//if the opcode or content don't match
			if (parsed_reply[2].compare( i == 0 ? OPCODE_SERVER_TO_CLIENT_ACK_SK : OPCODE_SERVER_TO_CLIENT_ACK_BOND)
					|| parsed_reply[4].compare(CONTENT_ACK))
				return RET_VAL_TO_UI_SERVER_SERVER_SENT_UNKNOWN_REPLY;
		}
	}//for (send sk and bond)

	if (!isSendToCA) //if we've sent to the server
	{
		//We've succesfuly sent the sk and bond to the server
		m_program_state = OPERATIONAL;
		return RET_VAL_TO_UI_SERVER_SERVER_RECEIVED_SK_AND_BOND;
	}

    //THIS POINT IS REACHED ONLY IF WE'VE SEND DATA TO THE CA:

	//asking the CA to validate the SK and Bond:
	content = CONTENT_VALIDATE;
	//create a message asking the CA to validate the bond
	msgToSend = createMessage(m_ID, CA_NAME,
	OPCODE_CLIENT_TO_CA_VALIDATE_BOND, content.size(), content);

	SocketWrapper sock_to_server(m_CA_IP, SERVER_AND_CA_TCP_PORT_NUM); //open a sock
	sock_to_server.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the request

	vector<string> parsed_reply = readAndParseMessageFromSocket(
			sock_to_server); //receive the reply

	sock_to_server.closeSocket(); //close the socket
	//GOT A REPLY.
	//checking the reply:

	//if the opcode is incorrect
	if (parsed_reply[2].compare(OPCODE_CA_TO_CLIENT_REPLY_VALIDATE_BOND))
		return RET_VAL_TO_UI_SERVER_CA_SENT_UNKNOWN_REPLY;

	//if the opcode is correct and the CA has approved
	if (!parsed_reply[4].compare(CONTENT_VALID))
	{
		m_program_state = GOT_CA_APPROVAL;
		return RET_VAL_TO_UI_SERVER_CA_APPROVED_SK_AND_BOND;
	}
	else
		//the CA didn't approve
		return RET_VAL_TO_UI_SERVER_CA_DISAPPROVED_SK_AND_BOND;
}//end of UI_Callback_SendSK_AndBondToCA()


/*
 * On UI request, sends an SM request to the server
 */
int ClientMachine::UI_Callback_requestSM_FromServer()
{
	string sm_request;
	string content(CONTENT_SM_REQUEST);

	//create a message header:
	sm_request = createMessage(m_ID,SERVER_NAME,OPCODE_CLIENT_TO_SERVER_REQUEST_SM,content.size(),CONTENT_SM_REQUEST);

	SocketWrapper sock_to_server(m_ServerIP,SERVER_AND_CA_TCP_PORT_NUM); //open a sock
	sock_to_server.sendToSocket(sm_request.c_str(),sm_request.size()); //send the request

	vector<string> parsed_reply = readAndParseMessageFromSocket(sock_to_server); //receive the reply

	sock_to_server.closeSocket(); //close the socket
	//GOT A REPLY.
	//checking the reply:

	//if the reply doesn't match the expected reply:
	if (parsed_reply[0].compare(SERVER_NAME) || parsed_reply[1].compare(m_ID) ||
			parsed_reply[2].compare(OPCODE_SERVER_TO_CLIENT_OR_CA_REPLY_SM))
	{
		return RET_VAL_TO_UI_SERVER_SM_NOT_RECEIVED;
	}

	int num_of_states = m_serializer->getNumOfStatesInStateMachineFromSerialized(parsed_reply[4]); //get the num of states @ SM
	m_SM = new StateMachine(num_of_states, 0); //create a new SM
	m_serializer->deserializeStateMachine(*m_SM,&m_virus,parsed_reply[4]);//deserialize the received SM

	m_program_state=NEED_CA_APPROVAL;  //update the state

	return RET_VAL_TO_UI_SERVER_SM_RECEIVED_OK;

}//end of UI_Callback_requestSM_FromServer()



/*
 * The client's main function.
 * Initiailizes the UI server on a new thread.
 */
void ClientMachine::run()
{
	bool isUI_ServerUp=false;

	//try to start the web server:
	for (int i=0; i<3 && !isUI_ServerUp ;i++)
		isUI_ServerUp = m_UI_Server->run();

	if (!isUI_ServerUp)
		cout << "UNABLE TO START THE CLIENT UI SERVER!";

}//end of run()


//Destructor
ClientMachine::~ClientMachine()
{
	if (m_EncHandler!=NULL)
		delete(m_EncHandler);

	delete(m_mapper);

	if (m_SM != NULL)
		delete m_SM;

	if (m_SK != NULL)
		delete m_SK;

	if (m_MSK != NULL)
		delete m_MSK;

	if (m_MSK != NULL)
		delete m_MSK;

	delete(m_serializer);
	delete(m_UI_Server);
}

