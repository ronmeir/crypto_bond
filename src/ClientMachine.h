/*
   _____ _ _            _     __  __            _     _
  / ____| (_)          | |   |  \/  |          | |   (_)
 | |    | |_  ___ _ __ | |_  | \  / | __ _  ___| |__  _ _ __   ___
 | |    | | |/ _ \ '_ \| __| | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
 | |____| | |  __/ | | | |_  | |  | | (_| | (__| | | | | | | |  __/
  \_____|_|_|\___|_| |_|\__| |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|


 * Defines the entire client machine: UI server + crypto_bond client.
 * The main entry point is run().
 * Receives requests from the user's UI client via the Client_UI_Server and performs the required
 * operations.
 */
#include "Constants.h"
#include "EncryptionHandler.h"
#include "StateMachine.h"
#include "WelcomeSocket.h"
#include "SocketWrapper.h"
#include "Client_UI_Server.h"
#include "ObjectSerializer.h"
#include "Messages.h"
#include "General_Funcs.h"
#include <string>
#include <cstdlib>
#include <sstream>

#ifndef CLIENTMACHINE_H_
#define CLIENTMACHINE_H_

class Client_UI_Server; //forward declaration of the Client_UI_Server

enum Client_UserState {CLIENT_NEED_STATE_MACHINE, CLIENT_NEED_CA_APPROVAL,
	CLIENT_OPERATIONAL, CLIENT_BUSTED };


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
	memberElement m_theSecret;
	EncryptionHandler::SK* m_SK;
	EncryptionHandler::MSK* m_MSK;
	EncryptionHandler::CT* m_Bond;
	ObjectSerializer* m_serializer;
	Client_UI_Server* m_UI_Server;
	Client_UserState m_program_state;

	//methods:
	int UI_Callback_requestSM_FromServer();
	int UI_Callback_CreateSK_AndBond();
	int UI_Callback_SendSK_AndBond(bool isSendToCA);
	int UI_Callback_SendMsg(std::string& servers_reply, std::string msg);

public:
	ClientMachine(const std::string,const std::string,const std::string);
	virtual ~ClientMachine();
	void run();
};

#endif /* CLIENTMACHINE_H_ */
