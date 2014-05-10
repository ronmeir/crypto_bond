/*
 * UserMachine.cpp
 *
 *  Created on: May 6, 2014
 *      Author: slava
 */

#include "ClientMachine.h"

using namespace std;

ClientMachine::ClientMachine(const string userID,const string ServerIP,const string CA_IP)
{
	m_ID.assign(userID);
	m_ServerIP.assign(ServerIP);
	m_CA_IP.assign(CA_IP);
	m_EncHandler = NULL;
	m_program_state = NEED_STATE_MACHINE;

}//end of constructor

/*
 * Creates an enc. handler based on the given state-machine
 */
void ClientMachine::setStateMachine (StateMachine* SM)
{
	if (m_EncHandler!=NULL)
		delete(m_EncHandler);

	m_EncHandler = new EncryptionHandler(PARAM_FILE_PATH,SM);
	m_program_state = NEED_CA_APPROVAL;
}//end of setStateMachine()



void ClientMachine::run()
{
	bool isWebServerUp=false;
	ClientWebServer webServer(CLIENT_WEB_SERVER_TCP_PORT_NUM);

	for (int i=0; i<3 && !isWebServerUp ;i++)
		isWebServerUp=webServer.run();

	if (!isWebServerUp)
		cout << "UNABLE TO START THE WEB SERVER!";

while(1);
}//end of run()



ClientMachine::~ClientMachine()
{
	if (m_EncHandler!=NULL)
		delete(m_EncHandler);
}

