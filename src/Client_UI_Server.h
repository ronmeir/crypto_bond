/*
 * Client_UI_Server.h
 * Defines a web server that is run by the client.
 * Provides the web-based UI for the project.
 */

#ifndef CLIENT_UI_SERVER_H_
#define CLIENT_UI_SERVER_H_

#include "BasicMultithreadedServer.h"
#include "ClientMachine.h"
#include "Constants.h"
#include <vector>


class Client_UI_Server : public BasicMultithreadedServer
{
private:
	//members:
	ClientMachine* m_clientMachine;

	//methods:
	int execOnWorkerThread(SocketWrapper, void* arg);
	static void* IntermediateWelcomeSocketThreadLauncher(void *);
	void handleRequestSM_FromServer(SocketWrapper& sock);
	void handleRequestToCreateSK_AndBond(SocketWrapper& sock);
	void handleRequestToSendSK_AndBondToCA (SocketWrapper& sock);
	void handleRequestToSendSK_AndBondToServer (SocketWrapper& sock);
	void handleRequestToSendMsgToServer (SocketWrapper& sock, std::string& msgToSend);
//	std::vector<string> readAndParseMessageFromUI (SocketWrapper& sock);

public:
	Client_UI_Server(ClientMachine* clientMachine, int tcp_port);
	virtual ~Client_UI_Server();
	int getPort ();
	bool run();
};

#endif /* Client_UI_Server */
