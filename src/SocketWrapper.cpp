/*
 * SocketWrapper.cpp
 *
 *  Created on: May 4, 2014
 *      Author: slava
 */

#include "SocketWrapper.h"

using namespace std;

/*
 * Creates a new socketWrapper that opens a socket to the given target address.
 * @param destIPaddr - a string with the destination IP addr.
 * @param dest_port - the dst. port num.
 */
SocketWrapper::SocketWrapper(string& destIPaddr, int dest_port)
{
	m_socketDiscrptr = InitSocket(destIPaddr,dest_port);
}//end of SocketWrapper(string)

/*
 * Creates a new socketWrapper with a given socket descriptor
 * @param socketDiscrptr - the socket descriptor
 */
SocketWrapper::SocketWrapper(int socketDiscrptr)
{
	m_socketDiscrptr = socketDiscrptr;
}//end of SocketWrapper(int)

/*
 * Returns the socket descriptor
 */
int SocketWrapper::getSocketDescriptor()
{
	return m_socketDiscrptr;
}//end of getSocketDescriptor()

/*
 * Tries to read the specified number of bytes from the socket.
 * Returns the actual amount of bytes that was read
 */
int SocketWrapper::receiveFromSocket(char* buffer, int numOfBytesToRead)
{
	if (m_socketDiscrptr==-1)
	{
		cout << "Invalid socket descriptor!";
		return -1;
	}

	 int  n = read(m_socketDiscrptr,buffer,numOfBytesToRead);
		if (n < 0)
		{
			 perror("ERROR reading from socket");
		}

	return n;
}//end of receiveFromSocket()

int SocketWrapper::sendToSocket (const char* buffer, int numOfBytesToSend)
{
	if (m_socketDiscrptr==-1)
	{
		cout << "Invalid socket descriptor!";
		return -1;
	}

   int n = write(m_socketDiscrptr,buffer,numOfBytesToSend);
    if (n < 0)
    {
         perror("ERROR writing to socket");
    }

    return n;

}//end of sendToSocket()

/*
 * Closes the socket
 */
void SocketWrapper::closeSocket()
{
	if (m_socketDiscrptr != -1)
	{
		close(m_socketDiscrptr);
		m_socketDiscrptr = -1;
	}
}//end of closeSocket()

/*
 * Establishes a TCP connection with a given destination IP.
 * param dest_ip - a string with the  destination IP
 * return - (-1) if the connection establishment failed, Socket number otherwise.
 */
int SocketWrapper::InitSocket (string& dest_ip, int dest_port)
{
  int connected_socket, sock;
  struct hostent *h;			/* info about server */
  struct sockaddr_in channel;		/* holds IP address */

  h = gethostbyname(dest_ip.c_str());		/* look up host's IP address */
  if (!h)
     {
      printf("gethostbyname() failed");
      return -1; //exit
     }

  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket < 0)
	{
	printf("Tracker socket failed!\n");
	return -1; //exit
	}

  memset(&channel, 0, sizeof(channel));

  channel.sin_family= AF_INET;

  memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);

  channel.sin_port = htons(dest_port);

  connected_socket = connect(sock, (struct sockaddr *) &channel, sizeof(channel));
  if (connected_socket < 0)
	{
	 return -1; //exit
	}

  return sock; //return the socket connected to the tracker
}//end of InitSocket()

SocketWrapper::~SocketWrapper()
{
}


