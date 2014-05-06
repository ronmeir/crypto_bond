/*
 * UserMachine.cpp
 *
 *  Created on: May 6, 2014
 *      Author: slava
 */

#include "UserMachine.h"

using namespace std;

UserMachine::UserMachine(const string userID,const string ServerIP,const string CA_IP)
{
	m_ID.assign(userID);
	m_ServerIP.assign(ServerIP);
	m_CA_IP.assign(CA_IP);

}

UserMachine::~UserMachine()
{
	// TODO Auto-generated destructor stub
}

