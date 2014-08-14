/*
   _____            __  __            _     _
  / ____|   /\     |  \/  |          | |   (_)
 | |       /  \    | \  / | __ _  ___| |__  _ _ __   ___
 | |      / /\ \   | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
 | |____ / ____ \  | |  | | (_| | (__| | | | | | | |  __/
  \_____/_/    \_\ |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|


 */

#include "CA_Machine.h"

//Constructor
CA_Machine::CA_Machine(string& Server_IP_addr) : BasicMultithreadedServer(g_selfPort)
{
	m_users = new map<string,CA_Machine::User>;  //the user DB
	m_Server_IP_addr = Server_IP_addr;
	m_mapper = new BilinearMappingHandler(PBC_PARAM_FILE_PATH); //we require a mapper to deserialize the SM string
	m_serializer = new ObjectSerializer(*m_mapper);

	printSplash();

	cout << "Trying to get an SM from the server." << endl;
	while (getSM_FromServer()==-1) //keep looping while the CA didn't receive a valid SM
	{
	}

	m_encHandlder = new EncryptionHandler(PBC_PARAM_FILE_PATH,m_SM,false);
	cout << "Got an SM from the Server!" << endl;
}//end of constructor

/*
 * @override
 * This method is launched to handle every incoming connection from a client that wishes to send a message
 * to the CA.
 * @param sock - a socket to the client
 * @param arg - an additional argument than can be passed by the worker thread dispatcher.
 */
int CA_Machine::execOnWorkerThread(SocketWrapper sock, void* arg)
{
	vector<string> parsed_request = readAndParseMessageFromSocket(sock); //receive the request
	string msgToSend;

	 //the client has sent an SK or Bond:
	if (!parsed_request[2].compare(OPCODE_CLIENT_TO_CA_SEND_SK) ||
			!parsed_request[2].compare(OPCODE_CLIENT_TO_CA_SEND_BOND))
	{
		handleClientSentSK_OrBond(parsed_request,sock);
	}//if we've received an SK or a Bond


	// the client asked us to validate it's bond and SK
	if (!parsed_request[2].compare(OPCODE_CLIENT_TO_CA_VALIDATE_BOND))
	{
		if (m_users->find(parsed_request[0]) != m_users->end()) //it's not a new user
		{
			CA_Machine::User user = (*m_users)[(parsed_request[0])]; //get the user from the DB

			if (user.state == CA_NEED_APPROVAL)
			{
				if (HandleSK_AndBondValidation(user)) //if the validation has passed
				{
					string msgToSend = createMessage(CA_NAME, parsed_request[0],
						 OPCODE_CA_TO_CLIENT_REPLY_VALIDATE_BOND,
							strlen(CONTENT_VALID), CONTENT_VALID); //generate a reply message

					sock.sendToSocket(msgToSend.c_str(), msgToSend.length()); //send a reply to the client

					updateTheServerWithClientDetails(parsed_request[0]); //update the server
				}
				else //validation has failed
				{
					 msgToSend = createMessage(CA_NAME, parsed_request[0],
						 OPCODE_CA_TO_CLIENT_REPLY_VALIDATE_BOND,
							strlen(CONTENT_INVALID), CONTENT_INVALID); //generate a reply message

					sock.sendToSocket(msgToSend.c_str(), msgToSend.length()); //send a reply to the client
				}
			}
			else //we still don't have both SK and Bond
			{
				 msgToSend = createMessage(CA_NAME, parsed_request[0],
					 OPCODE_CA_TO_CLIENT_REPLY_VALIDATE_BOND,
						strlen(CONTENT_NO_SK_OR_BOND), CONTENT_NO_SK_OR_BOND); //generate a reply message

				sock.sendToSocket(msgToSend.c_str(), msgToSend.length()); //send a reply to the client
			}
		}
		else //we haven't received anything from this user before the current message
		{
			 msgToSend = createMessage(CA_NAME, parsed_request[0],
				 OPCODE_CA_TO_CLIENT_REPLY_VALIDATE_BOND,
					strlen(CONTENT_NO_SK_AND_BOND), CONTENT_NO_SK_AND_BOND); //generate a reply message

				sock.sendToSocket(msgToSend.c_str(), msgToSend.length()); //send a reply to the client
		}


		sock.closeSocket();
	}//validate request

	return 0;
}//end of execOnWorkerThread()

/*
 * Handles the reception and storage of a message containing an SK or Bond
 * @param parsed_request - a string vector of all the message fields
 * @param sock - a socket to the client
 */
void CA_Machine::handleClientSentSK_OrBond(vector<string>& parsed_request, SocketWrapper sock)
{
	bool isSK = (!parsed_request[2].compare(OPCODE_CLIENT_TO_CA_SEND_SK)); //check if it's an SK or Bond

	isSK ? cout << "Receiving an SK from user: " << parsed_request[0] << endl :
			cout << "Receiving a Bond from user: " << parsed_request[0] << endl;

	if (m_users->find(parsed_request[0]) == m_users->end()) //if it's a new user
	{
		CA_Machine::User newUser;
		newUser.name = parsed_request[0];
		isSK ? newUser.SK = parsed_request[4] : newUser.Bond = parsed_request[4];

		int len = isSK ? newUser.SK.length() : newUser.Bond.length();

		newUser.state = isSK ? CA_NEED_BOND : CA_NEED_SK;
		//todo NOTE: OUR DB DOESN'T SUPPORT MULTIPLE USERS WITH THE SAME NAME.
		//WE ASSUME THAT A NAME IS A UNIQUE ID
		(*m_users)[newUser.name] = newUser; //insert the new user into the DB
	}
	else //it isn't a new user
	{
		CA_Machine::User user = (*m_users)[parsed_request[0]]; //get the user
		isSK ? user.SK = parsed_request[4] : user.Bond = parsed_request[4];

		if (isSK && user.state == CA_NEED_SK) //means we've already received a bond from this user
			user.state = CA_NEED_APPROVAL;

		if (!isSK && user.state == CA_NEED_BOND) //means we've already received a SK from this user
			user.state = CA_NEED_APPROVAL;

		int len = isSK ? user.SK.length() : user.Bond.length();

		(*m_users)[user.name] = user; //insert the new user into the DB
	}


	string msgToSend = createMessage(CA_NAME, parsed_request[0],
			isSK ? OPCODE_CA_TO_CLIENT_ACK_SK : OPCODE_CA_TO_CLIENT_ACK_BOND,
			strlen(CONTENT_ACK), CONTENT_ACK); //generate a reply message

	sock.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the response
	sock.closeSocket(); //close the socket

}//end of handleClientSentSK_OrBond()

/**
 * Checks if the client's SK and Bond are valid
 * @param user - the user
 * @return true of the decrypted bond matches the PT bond, false otherwise
 */
bool CA_Machine::HandleSK_AndBondValidation(CA_Machine::User& user)
{
	memberElement bondPT;
	memberElement decryptRes;

	m_encHandlder->getBilinearMappingHandler()->initEmptyMemberElementFromG1(bondPT); //init
	m_encHandlder->getBilinearMappingHandler()->initEmptyMemberElementFromG1(decryptRes); //init

	//construct a holder for the desirialized SK:
	EncryptionHandler::SK desirializedSK(m_encHandlder->getBilinearMappingHandler(),
			m_SM, NULL, false);

	m_serializer->deserializeSecretKey(desirializedSK, user.SK); //deserialize the SK

	//creating a new empty CT:
	EncryptionHandler::CT deserializedBond(m_encHandlder->getBilinearMappingHandler(),
			g_maxMessageLength, false);

	m_serializer->deserializeBond(deserializedBond, user.Bond); //deserialize

	m_encHandlder->completePartialEncryption(deserializedBond, m_virus);	//complete the enc.
	m_encHandlder->decrypt(decryptRes, desirializedSK, deserializedBond, *m_SM);  //decrypt

	if (m_serializer->hasBondInPlainText())
		m_serializer->getBondInPlainText(bondPT,true);   //get the PT bond
	else
		return false;


	if (!m_encHandlder->getBilinearMappingHandler()->compareElements(bondPT, decryptRes))
	{
		//cout << "Elements match!\n";
		cout << "Bond validation for user: " << user.name << " has succeeded!" << endl;
		return true;
	}
	else
	{
		cout << "Bond validation for user: " << user.name << " has failed!" << endl;
		//cout << "No match!\n";
		return false;
	}

}//end of validateClientSK_AndBond()

/*
 * Updates the Server with the details of an approved client (who's SK and Bond and valid)
 * @param clientDetails - we chose the pass the username
 */
int CA_Machine::updateTheServerWithClientDetails(string& clientDetails)
{
	SocketWrapper sockToServer(m_Server_IP_addr,g_serverPort);
	int i;

	for (i=0; sockToServer.getSocketDescriptor() == -1 && i<MAX_NUM_OF_SOCK_CONNECT_RETRIES ; i++)
	{
		if (sockToServer.getSocketDescriptor()==-1)
		{
			sockToServer.reconnect();
		}
	}

	if (i==MAX_NUM_OF_SOCK_CONNECT_RETRIES)
	{
		cout << "Unable to contact the Server and approve user: " << clientDetails << endl;
		return -1;
	}

	//at this point, sockToServer has to be valid.
	//ASSERT (sockToServer != -1)

	string msgToSend = createMessage(CA_NAME, SERVER_NAME,
			OPCODE_CA_TO_SERVER_APPROVE_CLIENT,
			clientDetails.length(), clientDetails.c_str()); //generate a reply message

	sockToServer.sendToSocket(msgToSend.c_str(),msgToSend.length()); //send
	sockToServer.closeSocket(); //close

	return 0;
}//end of updateTheServerWithClientDetails()

/*
 * The main entry point for this object
 */
void CA_Machine::run()
{

	runWelcomeSocket(NULL);   //start the welcome socket
}//end of run()

/*
 * Blocks the program until it receives an SM string from the server.
 * @returns 0 if we've received a valid SM, -1 otherwise.
 */
int CA_Machine::getSM_FromServer()
{
	SocketWrapper sockToServer(m_Server_IP_addr,g_serverPort);
	int ctr=0;

	while (sockToServer.getSocketDescriptor() == -1) //keep looping until we open a socket to the server
	{
		cout << "retry to connect";
		sleep(3000);
		sockToServer.reconnect(); //retry
		ctr++;

		if (ctr==3)
		{
			cout << "Unable to establish a connection to the Server! Continuing to try..." << endl;
			ctr=0;
		}
	}//while

	cout << "Established a connection with the Server!" << endl;
	cout << "Requesting a state machine from the Server." << endl;

	string msgToSend = createMessage(CA_NAME, SERVER_NAME,OPCODE_CA_TO_SERVER_REQUEST_SM,
			strlen(CONTENT_SM_REQUEST), CONTENT_SM_REQUEST);

	sockToServer.sendToSocket(msgToSend.c_str(), msgToSend.size()); //send the request

	vector<string> parsed_reply = readAndParseMessageFromSocket(sockToServer); //receive the reply

	sockToServer.closeSocket(); //close the socket

	//if the opcode doesn't match
	if (parsed_reply[2].compare(OPCODE_SERVER_TO_CLIENT_OR_CA_REPLY_SM))
	{
		cout << "Unknown server response!" << endl;
		return -1;
	}

	int num_of_states = m_serializer->getNumOfStatesInStateMachineFromSerialized(parsed_reply[4]); //get the num of states @ SM
	m_SM = new StateMachine(num_of_states, 0); //create a new SM
	m_serializer->deserializeStateMachine(*m_SM,&m_virus,parsed_reply[4]);//deserialize the received SM

	return 0;
}//end of getSM_FromServer();

void CA_Machine::printSplash()
{
cout <<	"_________     _____  " <<endl;
cout <<	"\\_   ___ \\   /  _  \\  " <<endl;
cout <<	"/    \\  \\/  /  /_\\  \\ " <<endl;
cout <<	"\\     \\____/    |    \\" <<endl;
cout <<	" \\______  /\\____|__  /" <<endl;
cout <<	"        \\/         \\/ " <<endl;
}

CA_Machine::~CA_Machine()
{
	if (m_encHandlder)
		delete(m_encHandlder);
	if (m_serializer)
		delete(m_serializer);
	if (m_users)
		delete(m_users);
	if (m_mapper)
		delete(m_mapper);
	if (m_SM)
		delete(m_SM);
}

