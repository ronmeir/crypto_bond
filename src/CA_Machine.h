/*
 * CA_Machine.h
 *
 *  Created on: Jul 2, 2014
 *      Author: slava
 */

#ifndef CA_MACHINE_H_
#define CA_MACHINE_H_

#include "BasicMultithreadedServer.h"
#include "EncryptionHandler.h"
#include "ObjectSerializer.h"
#include "StateMachine.h"
#include "Constants.h"
#include "Messages.h"
#include "General_Funcs.h"

#include <vector>
#include <string>

using namespace std;

enum CA_UserState { CA_NEED_BOND, CA_NEED_SK, CA_NEED_APPROVAL, CA_APPROVED };

class CA_Machine: public BasicMultithreadedServer
{
private:

		class User
		{
		public:
			std::string name;
			CA_UserState state;
			//saving the bond and SK as strings, to save memory space.
			string SK;
			string Bond;
		};

//members:
	EncryptionHandler* m_encHandlder;
	BilinearMappingHandler* m_mapper;
	StateMachine* m_SM;
	ObjectSerializer* m_serializer;
	std::map <string,User>* m_users;
	string m_Server_IP_addr;
	string m_virus;

	//methods:
	int execOnWorkerThread(SocketWrapper, void* arg);
	int getSM_FromServer();
	int updateTheServerWithClientDetails(string& clientDetails);
	bool HandleSK_AndBondValidation(CA_Machine::User& user);
	void handleClientSentSK_OrBond(vector<string>& parsed_request, SocketWrapper sock);

public:
	CA_Machine(string& Server_IP_addr);
	virtual ~CA_Machine();
	void run();

};


#endif /* CA_MACHINE_H_ */
