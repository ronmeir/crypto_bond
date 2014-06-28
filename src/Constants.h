/* Constants.h
 * Contains all the constant values used in the project.
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <string>
#include <cstdlib>
#include <sstream>
#include <vector>
#include "SocketWrapper.h"
#include "Messages.h"

using namespace std;


#define BUF_SIZE 8192
#define PARAM_FILE_PATH "./param/a.param" //the relative path of the parameter file
#define MAX_MSG_LENGTH 7
#define ALPHABET_SIZE 256
#define SYN_REQUEST_QUEUE_SIZE 10         //Max num of pending syn requests
#define SERVER_AND_CA_TCP_PORT_NUM 8000   //define a port number for the welcome sockets on the server and the CA
#define CLIENT_UI_SERVER_TCP_PORT_NUM 12345 //Client's UI port
#define MAX_ELEMENT_LENGTH_IN_BYTES 512
#define MAX_USER_NAME_LENGTH 25

#define RET_VAL_TO_UI_SERVER_SM_RECEIVED_OK 0
#define RET_VAL_TO_UI_SERVER_SM_NOT_RECEIVED 1

#define RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATED_OK 0
#define RET_VAL_TO_UI_SERVER_SK_AND_BOND_CREATE_FAILED 1

#define RET_VAL_TO_UI_SERVER_CA_APPROVED_SK_AND_BOND 0
#define RET_VAL_TO_UI_SERVER_CA_DISAPPROVED_SK_AND_BOND 1
#define RET_VAL_TO_UI_SERVER_CA_SENT_UNKNOWN_REPLY 2

#define RET_VAL_TO_UI_SERVER_SERVER_RECEIVED_SK_AND_BOND 0
#define RET_VAL_TO_UI_SERVER_SERVER_SENT_UNKNOWN_REPLY 1

#define RET_VAL_TO_UI_SERVER_SERVER_REPLY_OK 0

#define RET_VAL_TO_UI_SERVER


#endif
