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

string ClientMachine::UI_Callback_SendMsg(string msg)
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
		return "Unknown server response!";

	return parsed_reply[4];   //return the server's message content

}//end of UI_Callback_SendMsg()

/**
 * Initialize all required parameters and creates an SK and Bond.
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

	memberElement theMsgElem;

	//map the bond to some random element in G1
	m_EncHandler->mapStringToElementFromGT(theMsgElem, "BOND STRING");

	m_Bond = new EncryptionHandler::CT(m_EncHandler->getBilinearMappingHandler(),
			MAX_MSG_LENGTH, true);  //creating a new empty CT
	m_EncHandler->createPartialEncryption(*m_Bond,m_virus, theMsgElem); //generate a partial CT

	m_serializer->setSecretKey(*m_SK,*m_SM);  //set the SK into the serializer
	m_serializer->setBond(*m_Bond);			  //set the Bond into the serializer

	return RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATED_OK;

}//end of UI_Callback_CreateSK_AndBond()

/*
 * Sends the SK and Bond to the CA/Serve, waits for a reply on each one before sending the next.
 * Message order:
 * 1. Sends SK message and waits for an ACK.
 * 2. Sends Bond message and waits for an ACK.
 * 3. If the dst is the CA, sends a Validation-Request message and waits for a reply.
 */
int ClientMachine::UI_Callback_SendSK_AndBond(bool isSendToCA)
{
	string msgToSend, content;

	for (int i = 0; i < 2; i++) //repeat twice, once for the SK and once for the Bond
	{
		i==0?
		m_serializer->getSerializedSecretKeyString(content): //get the serialized SK
		m_serializer->getSerializedBondString(content);      //get the serialized Bond

		//create a message containing the SK/Bond:

		if (isSendToCA)  //if the message is to be sent to the CA
		{
		msgToSend = createMessage(m_ID, CA_NAME,
				i==0? OPCODE_CLIENT_TO_CA_SEND_SK : OPCODE_CLIENT_TO_CA_SEND_BOND,
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
			if (parsed_reply[2].compare( i == 0 ? OPCODE_CA_TO_CLIENT_ACK_SK : OPCODE_CA_TO_CLIENT_ACK_BOND)
					|| parsed_reply[4].compare(CONTENT_ACK))
				return RET_VAL_TO_UI_SERVER_CA_SENT_UNKNOWN_REPLY;

		}
		else  //the reply came from the server
		{
			//if the opcode or content don't match
			if (parsed_reply[2].compare( i == 0 ? OPCODE_SERVER_TO_CLIENT_ACK_SK : OPCODE_SERVER_TO_CLIENT_ACK_BOND)
					|| parsed_reply[4].compare(CONTENT_ACK))
				return RET_VAL_TO_UI_SERVER_SERVER_SENT_UNKNOWN_REPLY;

			m_program_state = OPERATIONAL;
			return RET_VAL_TO_UI_SERVER_SERVER_RECEIVED_SK_AND_BOND;
		}
	}//for

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
	if (parsed_reply[4].compare(CONTENT_VALID))
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
			parsed_reply[2].compare(OPCODE_SERVER_TO_CLIENT_REPLY_SM))
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
 * Reads a single message from a socket and parses it by fields.
 * @param sock - the socket
 * @return - a vector containing the message's fields (src,dst,opcode,size,content)
 */
vector<string> ClientMachine::readAndParseMessageFromSocket(SocketWrapper& sock)
{
	string currentField;
	char currentChar=0;
	vector<string> results;

	//TODO WE HAVE NO TIMEOUT ON THE READ ATTEMPT. CONSIDER ADDING IT.

	//reading the first 4 fields:
	for (int i=0; i<4 ;i++)
	{
		sock.receiveFromSocket(&currentChar,1); //read a single char

		while (currentChar != SFSC) //in the current message field
		{
			currentField.append(""+currentChar);    //append the char to a string
			sock.receiveFromSocket(&currentChar,1); //read the next char
		}//while

		results.push_back(currentField);
		currentField.clear();
	}//for

	int content_length = atoi(results[3].c_str()); //convert the length string to an int
	currentField.clear();

	//extract the content:
	for (int i=0; i<content_length ;i++)
	{
		sock.receiveFromSocket(&currentChar,1); //read the next char
		currentField.append(""+currentChar);    //append the char to a string
	}//for

	results.push_back(currentField); //add to the vector

	return results;

}//end of readAndParseMessageFromSocket()

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

