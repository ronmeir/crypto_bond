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
#include <sstream>

#ifndef CLIENTMACHINE_H_
#define CLIENTMACHINE_H_

class Client_UI_Server; //forward declaration of the Client_UI_Server

enum UserState {NEED_STATE_MACHINE, NEED_CA_APPROVAL, GOT_CA_APPROVAL, OPERATIONAL, BUSTED };

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
	std::vector<std::string> readAndParseMessageFromSocket(SocketWrapper& sock);
	int UI_Callback_requestSM_FromServer();
	int UI_Callback_CreateSK_AndBond();
	int UI_Callback_SendSK_AndBond(bool isSendToCA);
	std::string UI_Callback_SendMsg(std::string msg);

public:
	ClientMachine(const std::string,const std::string,const std::string);
	virtual ~ClientMachine();
	void run();
};

#endif /* CLIENTMACHINE_H_ */
