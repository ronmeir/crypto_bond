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
#include "StateMachineAndKey.pb.h"
#include "BilinearMappingHandler.h"
#include "StateMachine.h"
#include "EncryptionHandler.h"
#include "Constants.h"
#include "ClientMachine.h"
#include "ServerMachine.h"
#include "CA_Machine.h"
#include "ObjectSerializer.h"
#define DEBUG 0
#define CLIENT 1
#define SERVER 0
#define CA 	   0
#define SERVER_IP "127.0.0.1"

#define CA_IP "10.0.0.1"

using namespace std;                      //using the 'string' library

void debug_initializeStateMachine(StateMachine* machine);
void debug_mapperTest();
void debug_EncryptionTest(bool);
void compareCTs(EncryptionHandler::CT& ct1, EncryptionHandler::CT& ct2, BilinearMappingHandler* mapper);

//program parameters: machine type (user/server/ca), user id, server and CA IP (relevant for the user only)

int main()
{
	string ip;

#if CLIENT
	ClientMachine client("mr. user",SERVER_IP,CA_IP);
	client.run();

	while (1)   //keep the main thread running infinitely
	{
		sleep(1000); //done because sched_yield() simply won't work.
	}

	google::protobuf::ShutdownProtobufLibrary();
#endif
#if SERVER
	ip.assign(CA_IP);
	ServerMachine server (ip);
	server.run();
#endif
#if CA
	ip.assign(SERVER_IP);
	CA_Machine ca(ip);
	ca.run();
#endif

#if DEBUG

	//debug_mapperTest();
	debug_EncryptionTest(true);
#endif

	return 0;
}//end of main()

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

