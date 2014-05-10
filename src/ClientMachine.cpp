/*
 * UserMachine.cpp
 *
 *  Created on: May 6, 2014
 *      Author: slava
 */

#include "ClientMachine.h"

using namespace std;

ClientMachine::ClientMachine(const string userID,const string ServerIP,const string CA_IP)
			: BasicMultithreadedServer(CLIENT_WEB_SERVER_TCP_PORT_NUM)
{
	m_ID.assign(userID);
	m_ServerIP.assign(ServerIP);
	m_CA_IP.assign(CA_IP);

}//end of constructor

/*
 * Creates an enc. handler based on the given state-machine
 */
void ClientMachine::setStateMachine (StateMachine* SM)
{
 m_EncHandler = new EncryptionHandler(PARAM_FILE_PATH,SM);
}

ClientMachine::~ClientMachine()
{
	delete(m_EncHandler);
}

