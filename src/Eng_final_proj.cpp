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
#define CLIENT	0
#define SERVER	0
#define CA		0
#define HELP	1

#define SERVER_IP "127.0.0.1"
#define CA_IP "127.0.0.1"

using namespace std;

void debug_initializeStateMachine(StateMachine* machine);
void debug_mapperTest();
void debug_EncryptionTest(bool);
void compareCTs(EncryptionHandler::CT& ct1, EncryptionHandler::CT& ct2, BilinearMappingHandler* mapper);
void checkArgsForServerOrCA(char** argv);
bool checkIfPortIsValid(char* portString);
bool checkIfIPisValid(char* ipString);
int stringToInt (char* string);
void helpMenu();

/*
 * Terminal launching instructions:
 * To run as a Client: 	client [Server IP]:[Server port] [CA IP]:[CA port] <User Name>
 * To run as a Server: 	server [CA IP]:[CA port]
 * To run as a CA: 		ca [Server IP]:[Server port]
 */

int main(int argc, char** argv)
{
#if HELP
	helpMenu();
#endif

switch (argc)
	{
		case 1:
		{
			//program was called with no additional arguments. Let case 2 handle things.
		}//case 1
		case 2:
		{
			//A single argument, still invalid.
			//todo print the help menu and quit.
			exit(1);
			break;
		}//case 2
		case 3:
		{
			//Two arguments, has to be a Server or CA

			break;
		}//case 3
		case 4:
		{
			//Three arguments, has to be a Client

			break;
		}//case 3
		default:
		{
			//unsupported number of arguments
			//todo print the help menu and quit.
			exit(1);
		}

	}//end of switch






//#if CLIENT
//	ClientMachine client("Mr. user",SERVER_IP,CA_IP);
//	client.run();
//
//	while (1)   //keep the main thread running infinitely
//	{
//		sleep(1000); //done because sched_yield() simply won't work.
//	}
//
//	google::protobuf::ShutdownProtobufLibrary();
//#endif
//#if SERVER
//	ip.assign(CA_IP);
//	ServerMachine server (ip);
//	server.run();
//#endif
//#if CA
//	ip.assign(SERVER_IP);
//	CA_Machine ca(ip);
//	ca.run();
//#endif

	return 0;
}//end of main()

/**
 * Checks the user's arguments to see if they're compatible with a Server or CA requirements.
 * If the args are good, launches a CA or Server machine.
 * @param argv - the user's arguments.
 */
void checkArgsForServerOrCA(char** argv)
{
	if ()
}//end of checkArgsForServerOrCA()

/*
 * Checks if two string are equal, case insensitive.
 */
bool strCmpCaseInsensitive(char* a, char* b)
{
	for (;a && b; a++,b++)
	{
		if (tolower(*a) != tolower(*b) )
			return false;
	}

	if (a || b)			//if the string were of different lengths
		return false;

	return true;
}//end of strCmpCaseInsensitive()

/*
 * Checks for the theoretical validity of an IP address
 * @param ipString - the IP address in string form
 * @return true of the IP address appears to be OK, false otherwise
 */
bool checkIfIPisValid(char* ipString)
{
	const char delim[2] = ".";  //breaking the string down, using the decimal point as a delimiter
	int numOfSegments = 0;
	char *token;

	//todo add a function that breaks down a given string into a vector by given delimiters

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

void debug_EncryptionTest(bool isClient)
{
	//	debug_mapperTest();

	char* filePath = PARAM_FILE_PATH;
	string SK_str, SM_str, CT_str;

	//TODO MAKE SURE ALL OF THE FOLLOWIN 3 PARAMETERS MATCH:

//Server:
	StateMachine serverSM(6, 0); //a new machine with 6 states. initial state is '0'
	debug_initializeStateMachine(&serverSM); //init the machine
	string virus = "virus";

	EncryptionHandler srvrEncHand(filePath, &serverSM, false); //init enc. handler
	printf("EncryptionHandler is ready\n\n");
	ObjectSerializer serializer2(*srvrEncHand.getBilinearMappingHandler());  //new serializer (server)
	serializer2.setStateMachine(serverSM,virus);  //give the SM to the serializer
	serializer2.getSerializedStateMachineString(SM_str);  //serialize the SM

//Client:

	BilinearMappingHandler clientMapper(PARAM_FILE_PATH);
	ObjectSerializer serializer1(clientMapper);  //new serializer (client)
	int num_of_states = serializer1.getNumOfStatesInStateMachineFromSerialized(SM_str); //get the num of states @ SM

	StateMachine clientSM(num_of_states, 0); //create a new SM
	serializer1.deserializeStateMachine(clientSM,&virus,SM_str);//deserialize the received SM

	EncryptionHandler clientEncHand(filePath, &clientSM, true); //init enc. handler

	EncryptionHandler::MSK* msk = clientEncHand.setup(); //gen. master key
	EncryptionHandler::SK* sk = clientEncHand.keyGen();	//gen. secret key
	printf("keyGen is ready\n\n");

	memberElement theMsgElem;
	memberElement decryptRes;

	//map the bond to some random element in G1
	clientEncHand.mapStringToElementFromGT(theMsgElem, "BOND STRING");

	EncryptionHandler::CT bondToSend(clientEncHand.getBilinearMappingHandler(),
			MAX_MSG_LENGTH, true);  //creating a new empty CT
	clientEncHand.createPartialEncryption(bondToSend, virus, theMsgElem); //generate a partial CT

	serializer1.setSecretKey(*sk, serverSM); //set the SK
	serializer1.setBond(bondToSend);         //set the bond
	serializer1.getSerializedSecretKeyString(SK_str); //serialize
	serializer1.getSerializedBondString(CT_str);   //serialize


//Server:
	//construct a holder for the desirialized SK:
	EncryptionHandler::SK desirializedSK(
			srvrEncHand.getBilinearMappingHandler(), &serverSM, NULL, false);
	serializer2.deserializeSecretKey(desirializedSK, SK_str); //deserialize the SK


	EncryptionHandler::CT deserializedBond(
			srvrEncHand.getBilinearMappingHandler(), MAX_MSG_LENGTH, false); //creating a new empty CT

	serializer2.deserializeBond(deserializedBond, CT_str); //deserialize

//	compareCTs(cipherText,deserializedcipherText,clientEncHand.getBilinearMappingHandler());

	srvrEncHand.completePartialEncryption(deserializedBond, virus);	//complete the enc.
	srvrEncHand.decrypt(decryptRes, desirializedSK, deserializedBond,serverSM);  //decrypt

	BilinearMappingHandler* mapper = srvrEncHand.getBilinearMappingHandler();

	if (!mapper->compareElements(theMsgElem, decryptRes))
		cout << "Elements match!\n";
	else
		cout << "No match!\n";

//		memberElement toCompress;
//		mapper->initRandomMemberElementFromGT(toCompress);
//
//		int n = mapper->getElementLengthInBytes(toCompress,true);
//		unsigned char *data = (unsigned char*)malloc(n);
//		mapper->elementToByteArray(data, toCompress,true);
//
//		memberElement decompressed;
//		mapper->initEmptyMemberElementFromGT(decompressed);
//
//		mapper->byteArrayToElement(decompressed,data,true);
//
//		if (!mapper->compareElements(toCompress, decompressed))
//		    cout << "Elements match!\n";
//		else
//		    cout << "No match!\n";

	//	machineOfStates.moveToNextState('a');
	//	cout << serverSM.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('b');
	//	cout << serverSM.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('v');
	//	cout << serverSM.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('i');
	//	cout << serverSM.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('r');
	//	cout << serverSM.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('u');
	//	cout << serverSM.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('v');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('s');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	const std::vector<Transition3Tuple>* vect = serverSM.translateStateMachineToTriples();
	//	for (unsigned int i=0; i < vect->size() ;i++)
	//		cout << (*vect)[i].toString() << endl;
}

void debug_initializeStateMachine(StateMachine* machine)
{
	//TODO MAKE SURE THE INIT AND THE MACHINE SIZE MATCH
	int transitions[10][2];

	//state 0:
	transitions[0][0]='a';
	transitions[0][1]=0;
	transitions[1][0]='b';
	transitions[1][1]=0;
	transitions[2][0]='v';
	transitions[2][1]=1;

	machine->addState(0,transitions,3,false);

	//state 1:
	transitions[0][0]='a';
	transitions[0][1]=1;
	transitions[1][0]='b';
	transitions[1][1]=1;
	transitions[2][0]='i';
	transitions[2][1]=2;

	machine->addState(1,transitions,3,false);

	//state 2:
	transitions[0][0]='a';
	transitions[0][1]=2;
	transitions[1][0]='b';
	transitions[1][1]=2;
	transitions[2][0]='r';
	transitions[2][1]=3;

	machine->addState(2,transitions,3,false);

	//state 3:
	transitions[0][0]='a';
	transitions[0][1]=3;
	transitions[1][0]='b';
	transitions[1][1]=3;
	transitions[2][0]='u';
	transitions[2][1]=4;

	machine->addState(3,transitions,3,false);

	//state 4:
	transitions[0][0]='a';
	transitions[0][1]=4;
	transitions[1][0]='b';
	transitions[1][1]=4;
	transitions[2][0]='s';
	transitions[2][1]=5;

	machine->addState(4,transitions,3,false);

	//state 5:

	machine->addState(5,transitions,0,true);
printf("state machine is ready\n\n");
}//end of debug_initializeStateMachine


void debug_mapperTest()
{
	char* filePath = PARAM_FILE_PATH;
	memberElement g1, g2; //elements from the group
	expElement x, y;      //exponents
	memberElement temp1, temp2;
	memberElement powered1, powered2;

	BilinearMappingHandler mapper (filePath);

	mapper.initRandomMemberElementFromG1(g1);
	mapper.initEmptyMemberElementFromG1(g2);

	unsigned char data [1024];
	int n;

	n = mapper.getElementLengthInBytes(g1,false);
	mapper.elementToByteArray(data,g1,false);

	mapper.byteArrayToElement(g2,data,false);


	if (!mapper.compareElements(g1, g2))
	    cout << "Elements match!\n";
	else
	    cout << "No match!\n";


}//end of debug_mapperTest

void compareCTs(EncryptionHandler::CT& ct1, EncryptionHandler::CT& ct2, BilinearMappingHandler* mapper)
{

	if (!mapper->compareElements(ct1.m_C_start1,ct2.m_C_start1))
		cout << "Elements match!\n";
	else
		cout << "No match!\n";

	if (!mapper->compareElements(ct1.m_C_start2,ct2.m_C_start2))
		cout << "Elements match!\n";
	else
		cout << "No match!\n";

	if (!mapper->compareElements(ct1.m_C_end1,ct2.m_C_end1))
		cout << "Elements match!\n";
	else
		cout << "No match!\n";

	if (!mapper->compareElements(ct1.m_C_end2,ct2.m_C_end2))
		cout << "Elements match!\n";
	else
		cout << "No match!\n";

	if (!mapper->compareElements(ct1.m_Cm,ct2.m_Cm))
		cout << "Elements match!\n";
	else
		cout << "No match!\n";


	for (int i = 0; i < MAX_MSG_LENGTH; i++)
	{
		for (int j = 0; j < ALPHABET_SIZE + 1; j++)
		{
			if (!mapper->compareElements(ct1.m_Ci[j][i], ct2.m_Ci[j][i]))
				cout << "Elements match!\n";
			else
				cout << "No match!\n";
		}
	}

}

void helpMenu(){
	cout<<"==========================================================================="<<endl;
	cout<<"\t\t\t**************************\n\t\t\t**welcome to CryptoBond!**\n\t\t\t**************************"<<endl;
	cout<<"---------------------------------------------------------------------------"<<endl;
	cout<<"\tthis is the program instruction:"<<endl;
	cout<<"----------------------------------------------------------------"<<endl;

	cout<<"\t to run as Client:"<<endl;
	cout<<"\t\tclient [Server IP]:[Server PORT] [CA IP]:[CA PORT] <Name>"<<endl;
	cout<<"\t\t\tex: client 10.0.0.1:12345 10.0.0.2:12346 mr.user"<<endl;
	cout<<"\t---------------------------------------------------------"<<endl;


	cout<<"\t to run as Server:"<<endl;
	cout<<"\t\tserver [CA IP]:[CA PORT]"<<endl;
	cout<<"\t\t\tex: server 10.0.0.2:12346"<<endl;
	cout<<"\t---------------------------------------------------------"<<endl;


	cout<<"\t to run as CA:"<<endl;
	cout<<"\t\tca [Server IP]:[Server PORT]"<<endl;
	cout<<"\t\t\tex: ca 10.0.0.1:12345"<<endl;
	cout<<"\t---------------------------------------------------------"<<endl;


	cout<<"\t-h help\t\tshow the help menu"<<endl;
	cout<<"==========================================================================="<<endl;
}//end of helpMenu

