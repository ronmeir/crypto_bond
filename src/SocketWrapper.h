/*
 * SocketWrapper.h
 * A wrapper that provides convenient socket creation the usage.
 */
#include <string>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>      /* standard unix functions, like getpid() */
#include <pthread.h>
#include <sys/types.h>   /* various type definitions, like pid_t*/
#include <string.h>      /* String library */
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

#ifndef SOCKETWRAPPER_H_
#define SOCKETWRAPPER_H_


class SocketWrapper
{
private:
	//Members
	int m_socketDiscrptr;


	int InitSocket (std::string& dest_ip, int dest_port);

public:
	//Methods
	SocketWrapper(std::string&, int);
	SocketWrapper(int);
	int getSocketDescriptor();
	int receiveFromSocket(char* buffer, int numOfBytesToRead);
	int sendToSocket (char* buffer, int numOfBytesToSend);
	void closeSocket();
	virtual ~SocketWrapper();
};



#endif /* SOCKETWRAPPER_H_ */
