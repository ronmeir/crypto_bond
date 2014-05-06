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

class UserMachine : public BasicMultithreadedServer
{
private:
	std::string m_ID, m_ServerIP, m_CA_IP;
	WelcomeSocket* m_WebSrvrWelcomeSocket;
	EncryptionHandler* m_EncHandler;

public:
	UserMachine(const std::string,const std::string,const std::string);
	virtual ~UserMachine();
	void run();
};

#endif /* USERMACHINE_H_ */
