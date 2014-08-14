//============================================================================
// Name        : Digital Bond - Circuit Approach
// Author      : Ron Meiry and Slava Ustinov
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include <cstdio>
#include <pbc.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include "StateMachineAndKey.pb.h"
#include "BilinearMappingHandler.h"
#include "StateMachine.h"
#include "EncryptionHandler.h"
#include "Constants.h"
#include "ClientMachine.h"
#include "ServerMachine.h"
#include "CA_Machine.h"
#include "ObjectSerializer.h"
#define CLIENT	"Client"
#define SERVER	"Server"
#define CA		"Ca"

using namespace std;

bool checkArgsForServerOrCA(char**, bool*);
bool checkArgsForClient(char**);
bool checkIfPortIsValid(char*);
bool checkIfIPisValid(char* ipString);
bool checkAndParseIPandPortString(char*,char*,int*);
bool strCmpCaseInsensitive(char*, char*);
int stringToInt (char*);
void helpMenu();

/*
 * Terminal launching instructions:
 * To run as a Client: 	client [Server IP]:[Server port] [CA IP]:[CA port] <User Name>
 * To run as a Server: 	server [Self port] [CA IP]:[CA port]
 * To run as a CA: 		ca [Self port] [Server IP]:[Server port]
 */

int main(int argc, char** argv)
{

switch (argc)
	{
		case 1:
		{
			//program was called with no additional arguments. Let case 2 handle things.
		}//case 1
		case 2:
		{
			//2 arguments, invalid.
		}//case 2
		case 3:
		{
			//3 arguments, invalid.
			helpMenu(); //print the help menu
			exit(1);
			break;
		}//case 3
		case 4:
		{
			//4 arguments, has to be a Server or CA
			bool isServer;
			if (checkArgsForServerOrCA(argv,&isServer)) //if the args are good
			{
				string destIP;
				if (isServer)
				{
					destIP = g_CA_IP;
					ServerMachine server(destIP);
					server.run();
				}
				else
				{
					destIP = g_serverIP;
					CA_Machine ca(destIP);
					ca.run();
				}
			}
			break;
		}//case 4
		case 5:
		{
			//5 arguments, has to be a Client
			if ( checkArgsForClient(argv) ) //if the args are good
			{
				cout << g_serverPort << "  " << g_CA_Port << endl;
				ClientMachine client(g_userName,g_serverIP, g_CA_IP);
				client.run();
			}

			break;
		}//case 5
		default:
		{
			//unsupported number of arguments
			helpMenu(); //print the help menu
			exit(1);
		}

	}//end of switch


	return 0;
}//end of main()


/**
 * Checks the user's arguments to see if they're compatible with the Client's requirements.
 * If the args are good, launches a Client machine.
 * @param argv - the user's arguments.
 * @return - whether the addresses are valid or not
 */
bool checkArgsForClient(char** argv)
{

	if (strCmpCaseInsensitive(argv[1], CLIENT)) //if this should be a client machine
	{

		//check the 2nd arg (server addr):
		if (!checkAndParseIPandPortString(argv[2],g_serverIP, &g_serverPort))//if the addr. is invalid
		{
			cout << "Invalid Server address! Please verify the IP and port." << endl;
				return false;
		}

		//check the 3rd arg (ca addr):
		if (!checkAndParseIPandPortString(argv[3],g_CA_IP, &g_CA_Port))//if the addr. is invalid
		{
			cout << "Invalid CA address! Please verify the IP and port." << endl;
				return false;
		}

		//save the 4th arg (name):
		if (strlen(argv[4]) > MAX_ELEMENT_LENGTH_IN_BYTES) //if the username is longer then supported
		{
			strncpy(g_userName,argv[4],MAX_USER_NAME_LENGTH);
		}
		else
		{
			strcpy(g_userName,argv[4]);
		}

		return true;
	}
	else
	{
		cout << "Invalid machine type!"<<endl;
		helpMenu();
		return false; //should never be reached
	}

}//end of checkArgsForClient()

/**
 * Checks the user's arguments to see if they're compatible with a Server or CA requirements.
 * If the args are good, launches a CA or Server machine.
 * @param argv - the user's arguments.
 * @param saveIsServerHere - the indication whether we need to run a server or not
 * @return - whether the address is valid or not
 */
bool checkArgsForServerOrCA(char** argv, bool* saveIsServerHere)
{

	if (strCmpCaseInsensitive(argv[1], SERVER)) //if this should be a server machine
	{
		*saveIsServerHere = true;

		if (checkIfPortIsValid(argv[2])) //check the self port
		{
			g_selfPort = stringToInt(argv[2]); //save the self port
		}
		else
		{
			cout << "Invalid self port number!" << endl;
			return false;
		}

		//check the 3rd arg:
		if (checkAndParseIPandPortString(argv[3], g_CA_IP, &g_CA_Port))
		{
			return true;
		}
		else
		{
			cout << "Invalid CA address! Please verify the IP and port."<< endl;
			return false;
		}
	}
	else // not a server
	{
		if (strCmpCaseInsensitive(argv[1], CA)) //if this should be a CA machine
		{
			*saveIsServerHere = false;

			if (checkIfPortIsValid(argv[2])) //check the self port
			{
				g_selfPort = stringToInt(argv[2]); //save the self port
			}
			else
			{
				cout << "Invalid self port number!" << endl;
				return false;
			}

			//check the 3rd arg:
			if (checkAndParseIPandPortString(argv[3], g_serverIP, &g_serverPort))
			{
				return true;
			}
			else
			{
				cout << "Invalid Server address! Please verify the IP and port."
						<< endl;
				return false;
			}
		} //if
	} //else

	//the machine type is invalid.

	cout << "Invalid machine type or a wrong amount of arguments!"<<endl;
	helpMenu();
	return false; //should never be reached
}//end of checkArgsForServerOrCA()

/*
 * Checks if two strings are equal, case insensitive.
 */
bool strCmpCaseInsensitive(char* a, char* b)
{
	for (;*a && *b; a++,b++)
	{
		if (tolower(*a) != tolower(*b) )
			return false;
	}

	if (*a || *b)			//if the string were of different lengths
	{
		return false;
	}

	return true;
}//end of strCmpCaseInsensitive()

/*
 * Receives an IP and port string (as was entered by the user via the cmd line), checks it and
 * parses it.
 * example for a valid string to be parsed: 127.0.0.1:8000
 * @param input - an input that matches the pattern in the example.
 * @param saveIPhere - the IP string will be saved here
 * @param savePortHere - the port number will be saved here
 */
bool checkAndParseIPandPortString(char* input, char* saveIPhere, int* savePortHere)
{
	const char delim[] = ":";  //breaking the string down, using the ':' as a delimiter
	char *token;

	char inputCpy[25];
	strcpy(inputCpy,input);

   /* get the first token */
   token = strtok(inputCpy, delim);  //get the IP string

   if (checkIfIPisValid(token)) //if the IP string is valid
   {
	   strcpy(saveIPhere,token); //save
   }
   else
   {
	   return false;
   }

   /*
    * Since the last function also used strtok, we've lost the state for the next token,
    * so we can't simply call strtok with NULL is the 1st arg
    */
   token = strtok( inputCpy+strlen(token)+1, delim); //get the port string
   if (checkIfPortIsValid(token)) //if the port string is valid
   {
	  *savePortHere = stringToInt(token); //save
   }
   else
   {
	   return false;
   }

   return true;
}//end of checkAndParseIPandPortString()

/*
 * Checks for the theoretical validity of an IP address
 * @param input - the IP address in string form
 * @return true of the IP address appears to be OK, false otherwise
 */
bool checkIfIPisValid(char* input)
{
	const char delim[] = ".";  //breaking the string down, using the decimal point as a delimiter
	int numOfSegments = 0;
	char *token;

	if (input==NULL)
		return false;

	char ipString[25];
	strcpy (ipString,input);

	if (strlen(ipString)>15) //max size for an IP string
	{
		return false;
	}

   /* get the first token */
   token = strtok(ipString, delim);

   while( token != NULL )
   {
	  numOfSegments++; //count the number of segments

	  int num = stringToInt(token); //convert the string to a number

	  if (num < 0 || num > 255)
	  {
			cout << "Invalid IP address!" << endl;
			return false;
	  }

	  token = strtok(NULL, delim); //get the next token
   }
   //so far, all the read segments were valid.

   if (numOfSegments!=4)  //if the IP address doesn't contain 4 decimal-seperated values
   {
	   cout << "Invalid IP address!" << endl;
	   return false;
   }

   return true;
}//end of checkIfIPisValid()

/*
 * Checks a string contains a valid port number
 * @param portString - a string containing a port number
 * @return true if the port is within the legal range, false otherwise
 */
bool checkIfPortIsValid(char* portString)
{
	if (portString == NULL)
		return false;

	if (strlen(portString) > 5) //max len for a port string
	{
		return false;
	}

	char* ptr = portString;
	for (; *ptr ; ptr++) //making sure the string contains digits only
	{
		if (*ptr > 57 || *ptr < 48)
		{
			return false;
		}
	}

	int port = stringToInt(portString);

	if (port < 0 || port > 65536)
	{
		cout << "Invalid port number!" << endl;
		return false;
	}

	return true;
}//end of checkIfPortIsValid()


/*
 * Converts a string to an int
 * @param string - the string to be converted
 * @return - the conversion of the string to an int
 */
int stringToInt (char* string)
{
	int numb;
	istringstream ( string ) >> numb;
	return numb;
}//end of stringToInt()


void helpMenu(){
	cout<<"==========================================================================="<<endl;
	cout<<"\t\t\t**************************\n\t\t\t**welcome to CryptoBond!**\n\t\t\t**************************"<<endl;
	cout<<"---------------------------------------------------------------------------"<<endl;
	cout<<"\tthis is the program instruction:"<<endl;
	cout<<"----------------------------------------------------------------"<<endl;

	cout<<"\t to run as Client:"<<endl;
	cout<<"\t\tclient [Server IP]:[Server PORT] [CA IP]:[CA PORT] [Name]"<<endl;
	cout<<"\tExample: client 10.0.0.1:12345 10.0.0.2:12346 mr.user"<<endl;
	cout<<"\t-> Please note that the maximum user-name length is "<< MAX_USER_NAME_LENGTH << " chars!"<<endl;
	cout<<"\t---------------------------------------------------------"<<endl;


	cout<<"\t to run as Server:"<<endl;
	cout<<"\t\tserver [Self port] [CA IP]:[CA PORT]"<<endl;
	cout<<"\tExample: server 8000 10.0.0.2:12346"<<endl;
	cout<<"\t---------------------------------------------------------"<<endl;


	cout<<"\t to run as CA:"<<endl;
	cout<<"\t\tca [Self port] [Server IP]:[Server PORT]"<<endl;
	cout<<"\tExample: ca 8000 10.0.0.1:12345"<<endl;
	cout<<"\t---------------------------------------------------------"<<endl;


//	cout<<"\t-h help\t\tshow the help menu"<<endl;
	cout<<"==========================================================================="<<endl;
}//end of helpMenu


