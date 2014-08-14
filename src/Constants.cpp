#include "Constants.h"

//Global Vars:
char g_serverIP[16], g_CA_IP[16], g_userName[MAX_USER_NAME_LENGTH+1];
int g_serverPort, g_CA_Port;
int g_selfPort;
pthread_t g_GUI_threadId=0;
//end of Global Vars:
