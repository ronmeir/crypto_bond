/*
 * UserMachine.h
 * Defines the entire client machine.
 */
#include "Constants.h"
#include "EncryptionHandler.h"
#include "StateMachine.h"
#include "WelcomeSocket.h"
#include "SocketWrapper.h"
#include <string>

#ifndef USERMACHINE_H_
#define USERMACHINE_H_

enum UserState {NEED_STATE_MACHINE, NEED_CA_APPROVAL, GOT_CA_APPROVAL, BUSTED };

class UserMachine
{
private:
	std::string m_ID, m_ServerIP, m_CA_IP;
	WelcomeSocket m_WebSrvrWelcomeSocket

public:
	UserMachine();
	virtual ~UserMachine();
};

#endif /* USERMACHINE_H_ */
