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
#include "ObjectSerializer.h"
#define DEBUG 1

using namespace std;                      //using the 'string' library

void debug_initializeStateMachine(StateMachine* machine);
void debug_mapperTest();
void debug_EncryptionTest(bool);

//program parameters: machine type (user/server/ca), user id, server and CA IP (relevant for the user only)

int main()
{
#if !DEBUG
	printf("running....");
	ClientMachine client("user","ip1","ip2");
	client.run();

	while (1)   //keep the main thread running infinitely
	{
		sleep(1000); //done because sched_yield() simply won't work.
	}

	google::protobuf::ShutdownProtobufLibrary();
#else
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
		StateMachine machineOfStates(6,0); //a new machine with 6 states. initial state is '0'
		debug_initializeStateMachine(&machineOfStates); //init the machine
		const string virus= "virus";

		EncryptionHandler encHand(filePath,&machineOfStates,isClient); //init enc. handler
		printf("EncryptionHandler is ready\n\n");

		EncryptionHandler::MSK* msk = encHand.setup(); //gen. master key
		EncryptionHandler::SK* sk = encHand.keyGen();	//gen. secret key
		printf("keyGen is ready\n\n");
		memberElement theMsgElem;
		memberElement decryptRes;

		//Serialization test:
		ObjectSerializer serializer1(encHand);  //new serializer
		ObjectSerializer serializer2(encHand);  //new serializer

		serializer1.setSecretKey(*sk,machineOfStates); //set the SK
		serializer1.setStateMachine(machineOfStates,virus); //set the SM

		//construct a holder for the desirialized SK:
		EncryptionHandler::SK desirializedSK(encHand.getBilinearMappingHandler(),&machineOfStates, msk, false);

		serializer1.getSerializedSecretKeyString(SK_str); //serialize
		serializer1.getSerializedStateMachineString(SM_str); //serialize

		serializer2.deserializeSecretKey(desirializedSK,SK_str);  //deserialize the SK
		int num_of_states = serializer2.getNumOfStatesInStateMachineFromSerialized(SM_str);
		StateMachine deserializedmachineOfStates(num_of_states,0);  //create a new SM
		serializer2.deserializeStateMachine(deserializedmachineOfStates,SM_str);//deserialize the SM
		//done with the SK serialization test

//		if (!encHand.getBilinearMappingHandler()->compareElements(sk->m_K_start1,desirializedSK.m_K_start1))
//		    cout << "Elements match!\n";
//		else
//		    cout << "No match!\n";

		//map the bond to some random element in G1
		encHand.mapStringToElementFromGT(theMsgElem,"BOND STRING");

		EncryptionHandler::CT cipherText(msk,MAX_MSG_LENGTH);  //creating a new empty CT
		encHand.createPartialEncryption(cipherText,virus,theMsgElem);  //generate a partial CT
		encHand.completePartialEncryption(cipherText,virus);		//complete the enc.
		encHand.decrypt(decryptRes,desirializedSK,cipherText,deserializedmachineOfStates);  //decrypt

		BilinearMappingHandler* mapper = encHand.getBilinearMappingHandler();

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
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('b');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('v');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('i');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('r');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('u');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('v');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	machineOfStates.moveToNextState('s');
	//	cout << machineOfStates.toString() << "\n";
	//
	//	const std::vector<Transition3Tuple>* vect = machineOfStates.translateStateMachineToTriples();
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



