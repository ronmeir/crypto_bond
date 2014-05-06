/*
 * WelcomeSocket.cpp
 *
 *  Created on: May 4, 2014
 *      Author: slava
 */

#include "WelcomeSocket.h"

/*
 * Constructs a new welcome socket that listens to the specified port
 * @param port - the port number for this socket
 */
WelcomeSocket::WelcomeSocket(int port)
{
	m_socketDiscrptr = initWelcomeSocket(port);
}

/*
 * Returns the socket descriptor
 */
int WelcomeSocket::getSocketDescriptor()
{
	return m_socketDiscrptr;
}//end of getSocketDescriptor()

/*
 * Closes the socket
 */
void WelcomeSocket::closeSocket()
{
	if (m_socketDiscrptr != -1)
	{
		close(m_socketDiscrptr);
		m_socketDiscrptr = -1;
	}
}//end of closeSocket()

/*
 * Returns a SocketWrapper with a descriptor of a newly created socket
 */
SocketWrapper WelcomeSocket::acceptNewConnection()
{
	int sa=-1;
	if (m_socketDiscrptr != -1)
	{
		sa = accept(m_socketDiscrptr, 0, 0); /* block for connection request */
		if (sa < 0)
		{
			printf("Welcome socket accept failed!\n");
		}
	}

    return SocketWrapper(sa);
}//end of accept()

/*
 *Initializes a welcome socket
 * @param WebPort - A string with the port this we server will listen to
 */
int WelcomeSocket::initWelcomeSocket (int port)
{
  int sock, b, l;
  struct sockaddr_in channel;		/* holds IP address */

  /* Build address structure to bind to socket. */
  memset(&channel, 0, sizeof(channel));	/* zero channel */
  channel.sin_family = AF_INET;
  channel.sin_addr.s_addr = htonl(INADDR_ANY);
  channel.sin_port = htons(port);

  /* Passive open. Wait for connection. */
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
  if (sock < 0)
        {
         printf("Welcome socket failed!\n");
         return -1;
        }

  b = bind(sock, (struct sockaddr *) &channel, sizeof(channel));
  if (b < 0)
        {
         printf("WEB server's welcome-socket bind failed! Make sure the program is run by a SU!\n");
         return -1;
        }

  l = listen(sock, SYN_REQUEST_QUEUE_SIZE);		/* specify queue size */
  if (l < 0)
        {
        printf("Welcome socket listen failed!\n");
        return -1;
        }

  return sock;
} //CreateWebServerWelcomeSocketThread()

WelcomeSocket::~WelcomeSocket()
{
	closeSocket();
}

