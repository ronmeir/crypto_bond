/*
 * ServerMachine.h
 *
 *  Created on: Jun 22, 2014
 *      Author: slava
 */

#ifndef SERVERMACHINE_H_
#define SERVERMACHINE_H_

#include "BasicMultithreadedServer.h"
#include "EncryptionHandler.h"
#include "ObjectSerializer.h"
#include "StateMachine.h"
#include "Constants.h"
#include "Messages.h"
#include "General_Funcs.h"

using namespace std;

enum Server_UserState {SERVER_NEED_SK_AND_BOND,SERVER_NEED_BOND,SERVER_NEED_SK,
	SERVER_OPERATIONAL,SERVER_BUSTED };

class ServerMachine: public BasicMultithreadedServer
{

private:

	class User
	{
	public:
		string name;
		Server_UserState state;
		//saving the bond and SK as strings, to save memory space.
		string SK;
		string Bond;
	};

	//members:
	EncryptionHandler* m_encHandlder;
	StateMachine* m_SM;
	ObjectSerializer* m_serializer;
	string m_SM_string;
	string m_CA_IP_addr;
	std::map <string,User>* m_users;

	//methods:
	int execOnWorkerThread(SocketWrapper, void* arg);
	void initializeStateMachine(StateMachine* machine);
	void handleSM_request (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleCA_userApproval (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleClientSK (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleClientBond (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleClientMessage (vector<string>& incomingMsg,SocketWrapper& sock);
	void recoverBond (string& userName, string& virus);


public:
	ServerMachine(string& CA_IP_addr);
	virtual ~ServerMachine();
	int getPort ();
	void run();
};

#endif /* SERVERMACHINE_H_ */
