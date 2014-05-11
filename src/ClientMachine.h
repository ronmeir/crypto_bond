/*
 * ClientMachine.h
 * Defines the entire client machine: Web server + crypto_bond client
 */
#include "Constants.h"
#include "EncryptionHandler.h"
#include "StateMachine.h"
#include "WelcomeSocket.h"
#include "SocketWrapper.h"
#include "ClientWebServer.h"
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
	UserState m_program_state;

	//methods:
	void setStateMachine (StateMachine* SM);

public:
	ClientMachine(const std::string,const std::string,const std::string);
	virtual ~ClientMachine();
	void run();
};

#endif /* USERMACHINE_H_ */
