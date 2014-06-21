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
#include "ObjectSerializer.h"
#include "Messages.h"
#include <string>
#include <cstdlib>
#include <cstring>

#ifndef CLIENTMACHINE_H_
#define CLIENTMACHINE_H_

class Client_UI_Server; //forward declaration of the Client_UI_Server

enum UserState {NEED_STATE_MACHINE, NEED_CA_APPROVAL, GOT_CA_APPROVAL, BUSTED };

class ClientMachine
{
friend class Client_UI_Server;  //allow Client_UI_Server access to all vars and methods

private:
	//members:
	std::string m_ID, m_ServerIP, m_CA_IP;
	std::string m_virus;
	BilinearMappingHandler* m_mapper;
	EncryptionHandler* m_EncHandler;
	StateMachine* m_SM;
	EncryptionHandler::SK* m_SK;
	EncryptionHandler::MSK* m_MSK;
	EncryptionHandler::CT* m_Bond;
	ObjectSerializer* m_serializer;
	Client_UI_Server* m_UI_Server;
	UserState m_program_state;

	//methods:
	void setStateMachine (StateMachine* SM,bool isClient);
	std::vector<std::string> readAndParseMessageFromSocket(SocketWrapper& sock);

public:
	ClientMachine(const std::string,const std::string,const std::string);
	virtual ~ClientMachine();
	void run();
};

#endif /* CLIENTMACHINE_H_ */
