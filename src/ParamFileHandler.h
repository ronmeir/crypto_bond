/*
 * ParamFileHandler.h
 *
 * Contains methods for reading the global parameters and state machine from the global_param_file
 */

#include <iostream>
#include <string>
#include <stdio.h>
#include "Constants.h"
#include "General_Funcs.h"
#include "ServerMachine.h"

#ifndef PARAMFILEHANDLER_H_
#define PARAMFILEHANDLER_H_

#define SM_START_LABEL "*SM Start*"
#define SM_END_LABEL "*SM End*"

class ParamFileHandler
{
private:
	string m_filePath;

public:
	ParamFileHandler(const char*);
	virtual ~ParamFileHandler();
	bool loadGlobalParamsFromFile();
	bool loadStateMachineFromFile(StateMachine&);
};


#endif /* PARAMFILEHANDLER_H_ */
