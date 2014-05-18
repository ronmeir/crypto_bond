/*
 * ObjectSerializer.cpp
 * Responsible for serializing and de-serializing the StateMachine, SecretKey and CipherText objects so
 * they can be easily sent and received via socket.
 */

#include "ObjectSerializer.h"

using namespace std;

ObjectSerializer::ObjectSerializer(EncryptionHandler& encHandler)
{
	m_encHandler = &encHandler;   //set a ptr to an encryption handler.

  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
}

/*
 * Takes a valid CT and converts it to the protocol-buffer's CT format (that format will be later
 * used for serialization).
 * IMPORTANT: SERIALIZATION WILL BE DONE TO PARTIAL-CTs ONLY! WE NEVER SEND A FULL CT OVER THE LINE!
 * @param CT - the partial CT to be serialized
 */
void ObjectSerializer::setBond (EncryptionHandler::CT& CT)
{
	BilinearMappingHandler* mapper = m_encHandler->getBilinearMappingHandler();  //get a ptr to a bilinear mapper
	int n;
	unsigned char data[512];
	string *C_i_1, *C_i_2;

	//Cm:
	n = mapper->getElementLengthInBytes(CT.m_Cm,true);  //get the num of bytes needed to represent Cm
	mapper->elementToByteArray(data,CT.m_Cm,true);     //convert
	data[n]='\0';            //modify data to be a valid cstring

	m_Bond.set_cm((char*)data);				//set Cm

	//Cstart1:
	n = mapper->getElementLengthInBytes(CT.m_C_start1,false);  //get the num of bytes needed to represent Cstart1
	mapper->elementToByteArray(data,CT.m_C_start1,false);     //convert
	data[n]='\0';            //modify data to be a valid cstring

	m_Bond.set_c_start1((char*)data);				//set Cstart1

	//Cstart2:
	n = mapper->getElementLengthInBytes(CT.m_C_start2,false);  //get the num of bytes needed to represent Cstart2
	mapper->elementToByteArray(data,CT.m_C_start2,false);     //convert
	data[n]='\0';            //modify data to be a valid cstring

	m_Bond.set_c_start2((char*)data);				//set Cstart2

	//Cend1:
	n = mapper->getElementLengthInBytes(CT.m_C_end1,false);  //get the num of bytes needed to represent Cend1
	mapper->elementToByteArray(data,CT.m_C_end1,false);     //convert
	data[n]='\0';            //modify data to be a valid cstring

	m_Bond.set_c_end1((char*)data);				//set Cend1

	//Cend2:
	n = mapper->getElementLengthInBytes(CT.m_C_end2,false);  //get the num of bytes needed to represent Cend2
	mapper->elementToByteArray(data,CT.m_C_end2,false);     //convert
	data[n]='\0';            //modify data to be a valid cstring

	m_Bond.set_c_end2((char*)data);				//set Cend2

	//Ci,1 and Ci,2:
	//i is the current column
	for (int i=0; i<MAX_MSG_LENGTH ;i++)
	{
		//adding Ci,1:
		n = mapper->getElementLengthInBytes(CT.m_Ci[0][i],false);  //get the num of bytes needed to represent Ci,1
		mapper->elementToByteArray(data,CT.m_Ci[0][i],false);     //convert
		data[n]='\0';            //modify data to be a valid cstring

		C_i_1 = m_Bond.add_c_i_1();      //get a new empty string
		C_i_1->assign((char*)data);				//set Ci,1

		//j is the current row
		for (int j=1; j<ALPHABET_SIZE+1 ;j++)
		{
			//adding all possible Ci,2:
			n = mapper->getElementLengthInBytes(CT.m_Ci[j][i],false);  //get the num of bytes needed to represent Ci,2
			mapper->elementToByteArray(data,CT.m_Ci[j][i],false);     //convert
			data[n]='\0';            //modify data to be a valid cstring

			C_i_2 = m_Bond.add_c_i_2();      //get a new empty string
			C_i_2->assign((char*)data);				//set Ci,2

		}//inner for
	}//outer for

}
/*
 * Takes a valid SM and converts it to the protocol-buffer's SM format (that format will be later
 * used for serialization).
 * @param SM - reference to a valid StateMachine
 * @param optionalVirusString - the virus string. "" means this field is unused
 */
void ObjectSerializer::setStateMachine (StateMachine& SM, string optionalVirusString)
{
	const vector<Transition3Tuple>*  allTransitions = SM.translateStateMachineToTriples(); //get the transition vec.
	int numOfStates = SM.getTotalNumOfStates();           //get the num of states

	//set:
	if (optionalVirusString!="")
		m_Machine.set_virus(optionalVirusString);

	//for every state:
	for (int i=0; i < numOfStates ;i++)
	{
		setSingleState(m_Machine.add_statevec(),SM,allTransitions,i);
	}
}//end of setStateMachine()

/*
 * Modifies a single state in the serializer to represent a state in the StateMachine
 * @param saveHere - the state in the serializer to be updated
 * @param SM - reference to the stateMachine
 * @param allTransitions - the machine's transitions vector
 * @param stateID - the current state's ID
 */
void ObjectSerializer::setSingleState(StateMachineAndKey::StateMachine_State* saveHere, StateMachine& SM,
										const vector<Transition3Tuple>*  allTransitions, int stateID)
{
	//check if the current state is an acceptance state:
	SM.getIndexOfAcceptanceStateInTheAcceptanceStatesVector(stateID)==-1?
			saveHere->set_isacceptancestate(false) : saveHere->set_isacceptancestate(true);

	//setting all non-trivial transitions (X,Y,Sigma):

	//set the stateID (X):
	saveHere->set_stateid(stateID);

	int currentIndexInTransVector = stateID*ALPHABET_SIZE; //set the initial index to the beginning of the relevant
															//transitions block

	for (int j=0 ; j<ALPHABET_SIZE ;j++)
	{
		Transition3Tuple trans3tuple = allTransitions->at(currentIndexInTransVector); //get a transition
		if (trans3tuple.get_Y()!=stateID)  //if this is a non-trivial transition (not a self loop)
		{
			StateMachineAndKey::StateMachine_Transition* transInSerializer =
					saveHere->add_transitionvec();         //get a new, empty transition
			transInSerializer->set_input( ""+trans3tuple.get_Sigma() ); //set sigma
			transInSerializer->set_nextstate(trans3tuple.get_Y());  //set Y
		}
		currentIndexInTransVector++; //advance
	}//for
}//end of setSingleState()

ObjectSerializer::~ObjectSerializer()
{
	//Delete all global objects allocated by libprotobuf.
	 // google::protobuf::ShutdownProtobufLibrary();
}

