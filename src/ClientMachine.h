/*
 * UserMachine.h
 * Defines the entire client machine.
 */
#include "Constants.h"
#include "EncryptionHandler.h"
#include "StateMachine.h"
#include "WelcomeSocket.h"
#include "SocketWrapper.h"
#include "BasicMultithreadedServer.h"
#include <string>

#ifndef USERMACHINE_H_
#define USERMACHINE_H_

enum UserState {NEED_STATE_MACHINE, NEED_CA_APPROVAL, GOT_CA_APPROVAL, BUSTED };

class ClientMachine : public BasicMultithreadedServer
{
private:
	//members:
	std::string m_ID, m_ServerIP, m_CA_IP;
	EncryptionHandler* m_EncHandler;


	//methods:
	void setStateMachine (StateMachine* SM);

public:
	ClientMachine(const std::string,const std::string,const std::string);
	virtual ~ClientMachine();
	void run();
};

#endif /* USERMACHINE_H_ */
