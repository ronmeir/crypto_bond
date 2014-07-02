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
	std::string m_destIPaddr;
	int m_dest_port;

	int InitSocket (std::string& dest_ip, int dest_port);

public:
	//Methods
	SocketWrapper(std::string& destIPaddr, int dest_port);
	SocketWrapper(int socketDiscrptr);
	int getSocketDescriptor();
	int receiveFromSocket(char* buffer, int numOfBytesToRead);
	int sendToSocket (const char* buffer, int numOfBytesToSend);
	void closeSocket();
	void reconnect();
	virtual ~SocketWrapper();
};



#endif /* SOCKETWRAPPER_H_ */
