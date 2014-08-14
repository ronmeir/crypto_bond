/*
   _____                _              _
  / ____|              | |            | |
 | |     ___  _ __  ___| |_ __ _ _ __ | |_ ___
 | |    / _ \| '_ \/ __| __/ _` | '_ \| __/ __|
 | |___| (_) | | | \__ \ || (_| | | | | |_\__ \
  \_____\___/|_| |_|___/\__\__,_|_| |_|\__|___/


 * Contains all the constant values used in the project.
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <string>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <pthread.h>
#include <string>
#include "SocketWrapper.h"
#include "Messages.h"

#define DEBUG 0

using namespace std;

#define BUF_SIZE 5242880  					 	//define a max buffer size of 5 MiBi for all messages
#define PBC_PARAM_FILE_PATH "./param/a.param" 	//the relative path of the PBC parameter file
#define GLOBAL_PARAM_FILE_PATH "./param/global_param_file" 	    //the relative path of the Global parameter file
//#define VIRUS_STRING "virus"					//a string that is accepted by the SM
//#define MAX_MSG_LENGTH 10						//the maximal length of a message between a client and a server
#define ALPHABET_SIZE 256						//one of the enc. params
#define SYN_REQUEST_QUEUE_SIZE 10				//Max num of pending syn requests
//#define SERVER_TCP_PORT_NUM 8000   				//define a port number for the welcome socket on the server
//#define CA_TCP_PORT_NUM 8002   					//define a port number for the welcome socket on the CA
#define CLIENT_UI_SERVER_TCP_PORT_NUM 60001  	//Client's UI port
#define MAX_ELEMENT_LENGTH_IN_BYTES 512			//max size of a single compressed group element
#define MAX_USER_NAME_LENGTH 30
#define MAX_NUM_OF_SOCK_CONNECT_RETRIES 4


#define RET_VAL_TO_UI_SERVER_SM_RECEIVED_OK 0
#define RET_VAL_TO_UI_SERVER_SM_NOT_RECEIVED 1

#define RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATED_OK 0
#define RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATE_FAILED 1

#define RET_VAL_TO_UI_SERVER_CA_APPROVED_SK_AND_BOND 0
#define RET_VAL_TO_UI_SERVER_CA_DISAPPROVED_SK_AND_BOND 1
#define RET_VAL_TO_UI_SERVER_CA_DIDNT_RECEIVE_BOTH_SK_AND_BOND 5
#define RET_VAL_TO_UI_SERVER_CA_DIDNT_RECEIVE_SK_OR_BOND 4
#define RET_VAL_TO_UI_SERVER_CA_SENT_UNKNOWN_REPLY 2
#define RET_VAL_TO_UI_SERVER_CA_SERVICE_UNAVAILABLE 3

#define RET_VAL_TO_UI_SERVER_SERVER_RECEIVED_SK_AND_BOND 0
#define RET_VAL_TO_UI_SERVER_SERVER_SENT_UNKNOWN_REPLY 1
#define RET_VAL_TO_UI_SERVER_SERVER_DETECTED_VIRUS 2

#define RET_VAL_TO_UI_SERVER_SERVER_REPLY_OK 0

#define RET_VAL_TO_UI_SERVER_FAILED_TO_OPEN_A_SOCKET 10

//parameter names that should exist in the global_param_file:
#define PARAM_FROM_FILE_MAX_MSG_LEN "max_message_len"
#define PARAM_FROM_FILE_SM_SIZE "num_of_states_in_SM"
#define PARAM_FROM_FILE_VIRUS_STRING "virus_string"
//end of parameters

//These vars are defined at Constants.cpp:
extern char g_serverIP[];
extern char g_CA_IP[];
extern char g_userName[];
extern int g_serverPort, g_CA_Port;
extern int g_selfPort;
extern pthread_t g_GUI_threadId;
extern int g_maxMessageLength;
extern int g_stateMachineSize;
extern string g_virus_string;



#endif
