/*
 * ServerMachine.cpp
 *
 *  Created on: Jun 22, 2014
 *      Author: slava
 */

#include "ServerMachine.h"

ServerMachine::ServerMachine() : BasicMultithreadedServer(SERVER_AND_CA_TCP_PORT_NUM)
{
	m_SM = new StateMachine(6,0);  //creating a new SM with 6 states
	initializeStateMachine(m_SM); //init

	m_encHandlder = new EncryptionHandler(PARAM_FILE_PATH,m_SM,false);
	m_serializer = new ObjectSerializer(*m_encHandlder->getBilinearMappingHandler());
	m_users = new map<string,ServerMachine::User>;  //the user DB

}//end of constructor

/**
 * Handles all incoming connections to the server.
 */
int ServerMachine::execOnWorkerThread(SocketWrapper sock, void* arg)
{
	ServerMachine* this_machine = (ServerMachine*)arg;  //get a ptr to the running server_machine class instance

/* todo
 * Objectives:
 * 1. Handle SM request
 * 2. Handle CA client approvals
 * 3. Handle Client SK and Bond sendings
 * 4. Perform echo on incoming messages
 */
}//end of execOnWorkerThread()


void ServerMachine::run()
{
	runWelcomeSocket(this);   //launch the welcome socket
}//end of run()

void ServerMachine::initializeStateMachine(StateMachine* machine)
{
	//TODO MAKE SURE THE INIT AND THE MACHINE SIZE MATCH
	int transitions[10][2];

	//state 0:
	transitions[0][0]='a';
	transitions[0][1]=0;
	transitions[1][0]='b';
	transitions[1][1]=0;
	transitions[2][0]='v';
	transitions[2][1]=1;

	machine->addState(0,transitions,3,false);

	//state 1:
	transitions[0][0]='a';
	transitions[0][1]=1;
	transitions[1][0]='b';
	transitions[1][1]=1;
	transitions[2][0]='i';
	transitions[2][1]=2;

	machine->addState(1,transitions,3,false);

	//state 2:
	transitions[0][0]='a';
	transitions[0][1]=2;
	transitions[1][0]='b';
	transitions[1][1]=2;
	transitions[2][0]='r';
	transitions[2][1]=3;

	machine->addState(2,transitions,3,false);

	//state 3:
	transitions[0][0]='a';
	transitions[0][1]=3;
	transitions[1][0]='b';
	transitions[1][1]=3;
	transitions[2][0]='u';
	transitions[2][1]=4;

	machine->addState(3,transitions,3,false);

	//state 4:
	transitions[0][0]='a';
	transitions[0][1]=4;
	transitions[1][0]='b';
	transitions[1][1]=4;
	transitions[2][0]='s';
	transitions[2][1]=5;

	machine->addState(4,transitions,3,false);

	//state 5:

	machine->addState(5,transitions,0,true);
printf("state machine is ready\n\n");
}//end of debug_initializeStateMachine

int ServerMachine::getPort ()
{
	return m_TCP_PortNum;
}//end of getPort()

ServerMachine::~ServerMachine()
{
	delete(m_SM);
	delete(m_encHandlder);
	delete(m_serializer);
	delete(m_users);
}

