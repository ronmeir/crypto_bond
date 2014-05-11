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
#include "BilinearMappingHandler.h"
#include "StateMachine.h"
#include "EncryptionHandler.h"
#include "Constants.h"
#include "ClientMachine.h"

using namespace std;                      //using the 'string' library

void debug_initializeStateMachine(StateMachine* machine);
void debug_mapperTest();
void debug_EncryptionTest();

//program parameters: machine type (user/server/ca), user id, server and CA IP (relevant for the user only)

int main()
{
	ClientMachine client("user","ip1","ip2");
	client.run();

	while (1)   //keep the main thread eunning infinitely
	{
		sleep(1000); //done because sched_yield() simply won't work.
	}
	return 0;
}//end of main()

void debug_EncryptionTest()
{
	//	debug_mapperTest();

		char* filePath = PARAM_FILE_PATH;

		//TODO MAKE SURE ALL OF THE FOLLOWIN 3 PARAMETERS MATCH:
		StateMachine machineOfStates(6,0); //a new machine with 6 states. initial state is '0'
		debug_initializeStateMachine(&machineOfStates); //init the machine
		const string virus= "virus";

		EncryptionHandler encHand(filePath,&machineOfStates); //init enc. handler
		printf("EncryptionHandler is ready\n\n");

		const EncryptionHandler::MSK* msk = encHand.setup(); //gen. master key
		EncryptionHandler::SK* sk = encHand.keyGen();	//gen. secret key
		printf("keyGen is ready\n\n");
		memberElement theMsgElem;
		memberElement decryptRes;

		//map the bond to some random element in G1
		encHand.mapStringToElementFromGT(theMsgElem,"BOND STRING");

		//element_printf("%B\n", theMsgElem);

		EncryptionHandler::CT cipherText(msk,MAX_MSG_LENGTH);  //creating a new empty CT
		encHand.createPartialEncryption(cipherText,virus,theMsgElem);  //generate a partial CT
		encHand.completePartialEncryption(cipherText,virus);		//complete the enc.
		encHand.decrypt(decryptRes,*sk,cipherText,machineOfStates);  //decrypt

		BilinearMappingHandler* mapper = encHand.getBilinearMappingHandler();

		if (!mapper->compareElements(theMsgElem, decryptRes))
		    cout << "Elements match!\n";
		else
		    cout << "No match!\n";

		//TESTING THE COMPRESSION FEATURE FOR G1 ELEMENTS
	//
	//	memberElement toCompress;
	//	mapper->initRandomMemberElementFromG1(toCompress);
	//
	//	int n = mapper->getElementLengthInBytes(toCompress,false);
	//	unsigned char *data = (unsigned char*)malloc(n);
	//	mapper->elementToByteArray(data, toCompress,false);
	//
	//	memberElement decompressed;
	//	mapper->initEmptyMemberElementFromG1(decompressed);
	//
	//	mapper->byteArrayToElement(decompressed,data,false);
	//
	//	if (!mapper->compareElements(toCompress, decompressed))
	//	    cout << "Elements match!\n";
	//	else
	//	    cout << "No match!\n";

		//TESTING THE COMPRESSION FEATURE FOR GT ELEMENTS

		memberElement toCompress;
		mapper->initRandomMemberElementFromGT(toCompress);

		int n = mapper->getElementLengthInBytes(toCompress,true);
		unsigned char *data = (unsigned char*)malloc(n);
		mapper->elementToByteArray(data, toCompress,true);

		memberElement decompressed;
		mapper->initEmptyMemberElementFromGT(decompressed);

		mapper->byteArrayToElement(decompressed,data,true);

		if (!mapper->compareElements(toCompress, decompressed))
		    cout << "Elements match!\n";
		else
		    cout << "No match!\n";



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
	mapper.initRandomMemberElementFromG2(g2);
	mapper.initRandomExpElement(x);
	mapper.initRandomExpElement(y);
	mapper.initEmptyMemberElementFromGT(temp1);
	mapper.initEmptyMemberElementFromGT(temp2);
	mapper.initEmptyMemberElementFromG1(powered1);
	mapper.initEmptyMemberElementFromG2(powered2);

	mapper.power_Zn(powered1,g1,y);
	mapper.power_Zn(powered2,g2,x);
	mapper.bilinearMap(temp1,powered1,powered2);

	mapper.power_Zn(powered1,g1,y);
	mapper.power_Zn(powered2,g2,x);
	mapper.bilinearMap(temp2,powered1,powered2);

	if (!mapper.compareElements(temp1, temp2))
	    cout << "Elements match!\n";
	else
	    cout << "No match!\n";

	//mixing stuff up, so the elements shouldn't match:
	mapper.power_Zn(powered1,g1,x);
	mapper.power_Zn(powered2,g2,x);
	mapper.bilinearMap(temp2,powered1,powered2);

	if (!mapper.compareElements(temp1, temp2))
	    cout << "Elements match!\n" ;
	else
	    cout << "No match!\n";

	//checking the cpy func:
	mapper.element_cpy(temp1,temp2);

	if (!mapper.compareElements(temp1, temp2))
	    cout << "Elements match!\n" ;
	else
	    cout << "No match!\n";

}//end of debug_mapperTest



