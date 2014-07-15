/*
 *
   _____ _ _            _     _    _ _____    _____
  / ____| (_)          | |   | |  | |_   _|  / ____|
 | |    | |_  ___ _ __ | |_  | |  | | | |   | (___   ___ _ ____   _____ _ __
 | |    | | |/ _ \ '_ \| __| | |  | | | |    \___ \ / _ \ '__\ \ / / _ \ '__|
 | |____| | |  __/ | | | |_  | |__| |_| |_   ____) |  __/ |   \ V /  __/ |
  \_____|_|_|\___|_| |_|\__|  \____/|_____| |_____/ \___|_|    \_/ \___|_|


 * Client_UI_Server.h
 * The main entry point is run().
 * A part of the ClientMachine. The UI server is the one that handles all the
 * UI program's requests.
 */

#ifndef CLIENT_UI_SERVER_H_
#define CLIENT_UI_SERVER_H_

#include "BasicMultithreadedServer.h"
#include "ClientMachine.h"
#include "Constants.h"
#include <vector>


class Client_UI_Server : public BasicMultithreadedServer
{
#if DEBUG
	friend class ClientMachine;
#endif

private:
	//members:
	ClientMachine* m_clientMachine;
    bool isFirstSK_And_Bond_Send_req;

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
