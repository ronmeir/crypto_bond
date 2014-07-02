/*
 * CA_Machine.h
 *
 *  Created on: Jul 2, 2014
 *      Author: slava
 */

#ifndef CA_MACHINE_H_
#define CA_MACHINE_H_

#include "BasicMultithreadedServer.h"

using namespace std;

class CA_Machine: public BasicMultithreadedServer
{
public:
	CA_Machine();
	virtual ~CA_Machine();
};


#endif /* CA_MACHINE_H_ */
