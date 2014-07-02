/*
 * ServerMachine.cpp
 *
 *  Created on: Jun 22, 2014
 *      Author: slava
 */

#include "ServerMachine.h"

ServerMachine::ServerMachine() : BasicMultithreadedServer(SERVER_AND_CA_TCP_PORT_NUM)
{
	m_SM = new StateMachine(6,0);  //creating a new SM with 6 states
	initializeStateMachine(m_SM); //init

	m_encHandlder = new EncryptionHandler(PARAM_FILE_PATH,m_SM,false);
	m_serializer = new ObjectSerializer(*m_encHandlder->getBilinearMappingHandler());

	m_serializer->setStateMachine(*m_SM,VIRUS_STRING); //set the SM in the serializer
    m_serializer->getSerializedStateMachineString(m_SM_string); //serialize the SM

	m_users = new map<string,ServerMachine::User>;  //the user DB

}//end of constructor

/**
 * Handles all incoming connections to the server.
 */
int ServerMachine::execOnWorkerThread(SocketWrapper sock, void* arg)
{
	ServerMachine* this_machine = (ServerMachine*)arg;  //get a ptr to the running server_machine class instance

	vector<string> parsed_request = readAndParseMessageFromSocket(sock); //receive the request
/*
 * Objectives:
 * 1. Handle SM request
 * 2. Handle CA client approvals
 * 3. Handle Client SK and Bond transmissions
 * 4. Perform echo on incoming messages
 */
	//if the opcode means that it's an SM request
	if (!parsed_request[3].compare(OPCODE_CLIENT_TO_SERVER_REQUEST_SM) ||
			!parsed_request[3].compare(OPCODE_CA_TO_SERVER_REQUEST_SM))
	{
		handleSM_request(parsed_request,sock);
		return 0;
	}

	//if the CA sent us a client approval:
	if (!parsed_request[3].compare(OPCODE_CA_TO_SERVER_APPROVE_CLIENT) &&
			!parsed_request[0].compare(CA_NAME))
	{
		handleCA_userApproval(parsed_request,sock);
		return 0;
	}

	//if a client sent us an SK:
	if (!parsed_request[3].compare(OPCODE_CLIENT_TO_SERVER_SEND_SK))
	{
		handleClientSK(parsed_request,sock);
		return 0;
	}

	//if a client sent us a BOND:
	if (!parsed_request[3].compare(OPCODE_CLIENT_TO_SERVER_SEND_BOND))
	{
		handleClientBond(parsed_request,sock);
		return 0;
	}

	//if a client sent us a message:
	if (!parsed_request[3].compare(OPCODE_CLIENT_TO_SERVER_SEND_MSG))
	{
		//if the client exists in the DB
		if (m_users->find(parsed_request[0]) != m_users->end())
		{
			handleClientMessage(parsed_request, sock);
		}
		//todo create a reply for an unregistered client?
		return 0;
	}

	cout << "Received an unknown opcode: " + parsed_request[3] + "from user named: " + parsed_request[0] << endl;
	sock.closeSocket();
	return -1;

}//end of execOnWorkerThread()


void ServerMachine::handleSM_request (vector<string>& incomingMsg,SocketWrapper& sock)
{
	string msgToSend = createMessage(SERVER_NAME, incomingMsg[0],OPCODE_SERVER_TO_CLIENT_OR_CA_REPLY_SM,
			m_SM_string.length(), m_SM_string); //generate a reply message

	sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
	sock.closeSocket(); //close the socket

}//end of handleSM_request()

void ServerMachine::handleCA_userApproval (vector<string>& incomingMsg,SocketWrapper& sock)
{
	string msgToSend = createMessage(SERVER_NAME, CA_NAME,OPCODE_SERVER_TO_CA_ACK_APPROVE_CLIENT,
			strlen(CONTENT_ACK), CONTENT_ACK); //generate a reply message

	sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
	sock.closeSocket(); //close the socket

	//finished with the reply. Now we need to create a new User entry in our DB:

	ServerMachine::User newUser;
	newUser.name = "TEMP NAME. NEED TO DEFINE THE CONTENT PATTERN FOR A USER APPROVAL MESSAGE";
	newUser.state = NEED_SK_AND_BOND;

	//todo NOTE: OUR DB DOESN'T SUPPORT MULTIPLE USERS WITH THE SAME NAME.
	//WE ASSUME THAT A NAME IS A UNIQUE ID
	m_users->at(newUser.name) = newUser; //insert the new user into the DB

}//end of handleCA_userApproval()

void ServerMachine::handleClientSK (vector<string>& incomingMsg,SocketWrapper& sock)
{
	if (m_users->find(incomingMsg[0]) == m_users->end())
	{
		//this username doesn't exist in our DB. Send an appropriate reply:
		string msgToSend = createMessage(SERVER_NAME, incomingMsg[0],OPCODE_SERVER_TO_CLIENT_NOT_APPROVED_BY_CA,
				strlen(CONTENT_NO_CA_APPROVAL), CONTENT_NO_CA_APPROVAL); //generate a reply message

		sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
		sock.closeSocket(); //close the socket
	}
	else
	{
		//the user exists (it was already approved by the CA):
		string msgToSend = createMessage(SERVER_NAME, incomingMsg[0],OPCODE_SERVER_TO_CLIENT_ACK_SK,
				strlen(CONTENT_ACK), CONTENT_ACK); //generate a reply message

		sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
		sock.closeSocket(); //close the socket

		ServerMachine::User user = m_users->at(incomingMsg[0]);  //get the client from the DB
		user.SK = incomingMsg[4];  //save the SK string

		if (user.state == NEED_SK_AND_BOND) //if we've needed both SK and bond
			user.state = NEED_BOND;    //update the state

		if (user.state == NEED_SK)      //if we've needed only the SK
			user.state = OPERATIONAL;   //update the state

		m_users->at(incomingMsg[0]) = user;  //update the user's data in the DB
	}
}//end of handleClientSK()

void ServerMachine::handleClientBond (vector<string>& incomingMsg,SocketWrapper& sock)
{
	//if the user isn't in the DB
	if (m_users->find(incomingMsg[0]) == m_users->end())
	{
		//this username doesn't exist in our DB. Send an appropriate reply:
		string msgToSend = createMessage(SERVER_NAME, incomingMsg[0],OPCODE_SERVER_TO_CLIENT_NOT_APPROVED_BY_CA,
				strlen(CONTENT_NO_CA_APPROVAL), CONTENT_NO_CA_APPROVAL); //generate a reply message

		sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
		sock.closeSocket(); //close the socket
	}
	else
	{
		//the user exists (it was already approved by the CA):
		string msgToSend = createMessage(SERVER_NAME, incomingMsg[0],OPCODE_SERVER_TO_CLIENT_ACK_BOND,
				strlen(CONTENT_ACK), CONTENT_ACK); //generate a reply message

		sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
		sock.closeSocket(); //close the socket

		ServerMachine::User user = m_users->at(incomingMsg[0]);  //get the client from the DB
		user.Bond = incomingMsg[4];  //save the SK string

		if (user.state == NEED_SK_AND_BOND) //if we;'ve needed both SK and bond
			user.state = NEED_SK;    //update the state

		if (user.state == NEED_BOND)      //if we've needed only the bond
			user.state = OPERATIONAL;   //update the state

		m_users->at(incomingMsg[0]) = user;  //update the user's data in the DB
	}
}//end of handleClientBond()

void ServerMachine::handleClientMessage (vector<string>& incomingMsg,SocketWrapper& sock)
{
	bool isVirus = m_SM->checkStringForViruses(incomingMsg[4]); //check the msg for viruses
	string content, msgToSend;

	if (isVirus)
	{
		content = CONTENT_CLIENT_WAS_BUSTED + incomingMsg[0];  //create a reply content
		msgToSend = createMessage(SERVER_NAME, incomingMsg[0],OPCODE_SERVER_TO_CLIENT_VIRUS_DETECTED_IN_MSG,
				content.length(), content); //generate a reply message

		recoverBond(incomingMsg[0],incomingMsg[4]); //recover the bond
	}
	else
	{
		content = CONTENT_SERVER_ECHO_PREFIX + incomingMsg[4]; //create a reply content
		msgToSend = createMessage(SERVER_NAME, incomingMsg[0],OPCODE_SERVER_TO_CLIENT_ECHO_MSG,
				content.length(), content); //generate a reply message
	}

	//send:
	sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
	sock.closeSocket(); //close the socket
}//end of handleClientMessage()

void ServerMachine::recoverBond (string& userName, string& virus)
{
	memberElement decryptRes;
	//todo complete
	cout << "THE SERVER HAS DETECTED A VIRUS IN A MESSAGE FROM: " << userName << endl;

	ServerMachine::User user = m_users->at(userName); //extract the user from the DB

	//construct a holder for the desirialized SK:
	EncryptionHandler::SK desirializedSK(m_encHandlder->getBilinearMappingHandler(),
			m_SM, NULL, false);

	m_serializer->deserializeSecretKey(desirializedSK, user.SK); //deserialize the SK

	//creating a new empty CT:
	EncryptionHandler::CT deserializedBond(m_encHandlder->getBilinearMappingHandler(),
			MAX_MSG_LENGTH, false);

	m_serializer->deserializeBond(deserializedBond, user.Bond); //deserialize

	m_encHandlder->completePartialEncryption(deserializedBond, virus);	//complete the enc.
	m_encHandlder->decrypt(decryptRes, desirializedSK, deserializedBond, *m_SM);  //decrypt

	//the bond is located in decryptRes

}//end of recoverBond()

void ServerMachine::run()
{
	runWelcomeSocket(this);   //launch the welcome socket (the welcome socket doesn't run on a thread)
}//end of run()

void ServerMachine::initializeStateMachine(StateMachine* machine)
{
	//TODO MAKE SURE THE INIT AND THE MACHINE SIZE MATCH
	int transitions[10][2];

	//state 0:
	transitions[0][0]='a';
	transitions[0][1]=0;
	transitions[1][0]='b';
	transitions[1][1]=0;
	transitions[2][0]='v';
	transitions[2][1]=1;

	machine->addState(0,transitions,3,false);

	//state 1:
	transitions[0][0]='a';
	transitions[0][1]=1;
	transitions[1][0]='b';
	transitions[1][1]=1;
	transitions[2][0]='i';
	transitions[2][1]=2;

	machine->addState(1,transitions,3,false);

	//state 2:
	transitions[0][0]='a';
	transitions[0][1]=2;
	transitions[1][0]='b';
	transitions[1][1]=2;
	transitions[2][0]='r';
	transitions[2][1]=3;

	machine->addState(2,transitions,3,false);

	//state 3:
	transitions[0][0]='a';
	transitions[0][1]=3;
	transitions[1][0]='b';
	transitions[1][1]=3;
	transitions[2][0]='u';
	transitions[2][1]=4;

	machine->addState(3,transitions,3,false);

	//state 4:
	transitions[0][0]='a';
	transitions[0][1]=4;
	transitions[1][0]='b';
	transitions[1][1]=4;
	transitions[2][0]='s';
	transitions[2][1]=5;

	machine->addState(4,transitions,3,false);

	//state 5:

	machine->addState(5,transitions,0,true);
printf("state machine is ready\n\n");
}//end of debug_initializeStateMachine

int ServerMachine::getPort ()
{
	return m_TCP_PortNum;
}//end of getPort()

ServerMachine::~ServerMachine()
{
	delete(m_SM);
	delete(m_encHandlder);
	delete(m_serializer);
	delete(m_users);
}

