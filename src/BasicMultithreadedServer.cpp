/*
  ____            _        __  __       _ _   _ _   _                        _          _    _____
 |  _ \          (_)      |  \/  |     | | | (_) | | |                      | |        | |  / ____|
 | |_) | __ _ ___ _  ___  | \  / |_   _| | |_ _| |_| |__  _ __ ___  __ _  __| | ___  __| | | (___   ___ _ ____   _____ _ __
 |  _ < / _` / __| |/ __| | |\/| | | | | | __| | __| '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` |  \___ \ / _ \ '__\ \ / / _ \ '__|
 | |_) | (_| \__ \ | (__  | |  | | |_| | | |_| | |_| | | | | |  __/ (_| | (_| |  __/ (_| |  ____) |  __/ |   \ V /  __/ |
 |____/ \__,_|___/_|\___| |_|  |_|\__,_|_|\__|_|\__|_| |_|_|  \___|\__,_|\__,_|\___|\__,_| |_____/ \___|_|    \_/ \___|_|


 */

#include "BasicMultithreadedServer.h"

//constructor
BasicMultithreadedServer::BasicMultithreadedServer(int tcp_port)
{
	m_TCP_PortNum = tcp_port;
	m_welcomeSock = NULL;
}

/*
 * Launches a welcome-socket that fires up a worker thread for every incoming connection.
 * @param *fargForWorkerThread - an argument to be passed to the worker thread
 */
void BasicMultithreadedServer::runWelcomeSocket(void* argForWorkerThread)
{
	WelcomeSocket temp(m_TCP_PortNum);  //create a new welcome socket and start listening to the given port
	m_welcomeSock = &temp;
	SocketWrapper sa(-1);
	pthread_t threadId;
	int failCtr=0;

	  /* Welcome socket is now set up and bound. Wait for connection and process it. */
	while (1)
	{
		if (failCtr == 5)
		{
			Quit(1);
		}

		sa = m_welcomeSock->acceptNewConnection(); /* block for connection request */
		if (sa.getSocketDescriptor() < 0)
		{
			printf("Welcome socket accept failed!\n");
			failCtr++;
			continue;
		}

		arg_struct argz;
		argz.obj=static_cast<void*>(this);
		argz.arg = argForWorkerThread;
		argz.sockDescrptr=sa.getSocketDescriptor();

		if (pthread_create(&threadId, NULL,IntermediateWorkerThreadLauncher, &argz)) //create a new connection thread.
		{
			printf("Error while creating a worker thread!");
			return;
		}

		pthread_detach(threadId); //detach the thread

	} //while (1)

}//end of run()

/*
 * This method is run on a worker thread. It's used for allowing usage of this class's member methods
 * on a thread with pthread_create (which allows static or global methods only)
 *@param argz - a ptr to an arg_struct, containing a ptr to this class' instance and an additional argument for this func.
 */
void* BasicMultithreadedServer::IntermediateWorkerThreadLauncher(void* argz)
{
	   arg_struct *args = ( arg_struct *)argz; //contains the 2 arguments required for this method
	  //cast to a static instance, though it actually isn't. Need to make sure this instance is alive
	  //throughout the execution of this thread:
	  BasicMultithreadedServer *thisObj = static_cast<BasicMultithreadedServer*>(args->obj);
	  thisObj->execOnWorkerThread(SocketWrapper(args->sockDescrptr),args->arg);   //exec the abstract func

	  return (void*)0;
}//end of IntermediateWorkerThreadLauncher()

WelcomeSocket* BasicMultithreadedServer::getWelcomeSocketDescrptr()
{
	return m_welcomeSock;
}

BasicMultithreadedServer::~BasicMultithreadedServer()
{
}

