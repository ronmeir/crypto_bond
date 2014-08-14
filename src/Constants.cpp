#include "Constants.h"

//Global Vars:

// #### Received for the command line parameters:
char g_serverIP[16];   //the server's IP
char g_CA_IP[16];	   //the CA's IP
char g_userName[MAX_USER_NAME_LENGTH+1]; //relevant to a client only
int g_serverPort;      //the server's port
int g_CA_Port;		   //the CA's port
int g_selfPort;        //the port that the welcome socket will listen to (relevant to CA and server only)

// #### Read for the global_param_file:
int g_maxMessageLength = 0;	      //the maximal length of a message that can be send from a client to the server
							  //in the operational stage
int g_stateMachineSize = 0;       //the number of states in the state machine
string g_virus_string;		  //a single string that is recognized by the state machine as a virus

// #### misc:
pthread_t g_GUI_threadId=0;   //relevant to a client only

//end of Global Vars
