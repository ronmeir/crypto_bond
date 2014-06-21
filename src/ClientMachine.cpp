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

/*
 * Creates an enc. handler based on the given state-machine
 */
void ClientMachine::setStateMachine (StateMachine* SM,bool isClient)
{
	if (m_EncHandler!=NULL)
		delete(m_EncHandler);

	m_EncHandler = new EncryptionHandler(PARAM_FILE_PATH,SM,isClient);
	m_program_state = NEED_CA_APPROVAL;  //update the state
}//end of setStateMachine()

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

