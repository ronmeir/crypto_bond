/*
 * ClientMachine.h
 * Defines the entire client machine: Web server + crypto_bond client
 */
#include "Constants.h"
#include "EncryptionHandler.h"
#include "StateMachine.h"
#include "WelcomeSocket.h"
#include "SocketWrapper.h"
#include "Client_UI_Server.h"
#include <string>
#include <cstring>

#ifndef USERMACHINE_H_
#define USERMACHINE_H_

enum UserState {NEED_STATE_MACHINE, NEED_CA_APPROVAL, GOT_CA_APPROVAL, BUSTED };

class ClientMachine
{

private:
	//members:
	std::string m_ID, m_ServerIP, m_CA_IP;
	EncryptionHandler* m_EncHandler;
	Client_UI_Server* m_UI_Server;
	UserState m_program_state;

	//methods:
	void setStateMachine (StateMachine* SM,bool isClient);

public:
	ClientMachine(const std::string,const std::string,const std::string);
	virtual ~ClientMachine();
	void run();
};

#endif /* USERMACHINE_H_ */
