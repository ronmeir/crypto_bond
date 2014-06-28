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

enum UserState {NEED_SK_AND_BOND,NEED_BOND,NEED_SK,OPERATIONAL,BUSTED };

class ServerMachine: public BasicMultithreadedServer
{

private:

	class User
	{
	public:
		std::string name;
		UserState state;
		//saving the bond and SK as strings, to save memory space.
		string SK;
		string Bond;
	};

	//members:
	EncryptionHandler* m_encHandlder;
	StateMachine* m_SM;
	ObjectSerializer* m_serializer;
	string m_SM_string;
	std::map <string,User>* m_users;

	//methods:
	int execOnWorkerThread(SocketWrapper, void* arg);
	void initializeStateMachine(StateMachine* machine);
	void handleSM_request (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleCA_userApproval (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleClientSK (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleClientBond (vector<string>& incomingMsg,SocketWrapper& sock);
	void handleClientMessage (vector<string>& incomingMsg,SocketWrapper& sock);
	void recoverBond (string& userName);


public:
	ServerMachine();
	virtual ~ServerMachine();
	int getPort ();
	void run();
};

#endif /* SERVERMACHINE_H_ */
