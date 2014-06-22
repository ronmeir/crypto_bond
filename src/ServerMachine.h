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

using namespace std;

enum UserState {NEED_SK_AND_BOND,NEED_BOND,OPERATIONAL,BUSTED };

class ServerMachine: public BasicMultithreadedServer
{

private:

	class User
	{
	public:
		std::string name;
		UserState state;
		EncryptionHandler::SK* SK;
		EncryptionHandler::CT* Bond;
	};

	//members:
	EncryptionHandler* m_encHandlder;
	StateMachine* m_SM;
	ObjectSerializer* m_serializer;
	std::map <string,User>* m_users;

	//methods:
	int execOnWorkerThread(SocketWrapper, void* arg);
	void initializeStateMachine(StateMachine* machine);


public:
	ServerMachine();
	virtual ~ServerMachine();
	int getPort ();
	void run();
};

#endif /* SERVERMACHINE_H_ */
