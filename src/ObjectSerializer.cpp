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
	m_isBondSet = false;
	m_isSecretKeySet = false;
	m_isStateMachineSet = false;
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against:
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}//end of constructor

/*
 * Deserializes the SK string
 * @param saveHere - This SK should only contain allocated space that can be filled.
 * @param SK_string - the serialized SK
 */
void ObjectSerializer::deserializeSecretKey (EncryptionHandler::SK& saveHere,std::string SK_string)
{
	//TODO MAKE SURE THIS IS RUN ONLY WITH AN SK-SHELL (Server's SK)

	BilinearMappingHandler* mapper = m_encHandler->getBilinearMappingHandler(); //get the mapper
	m_SK.ParseFromString(SK_string);  //deserialize the SK

	//getting Kstart1:
	 mapper->byteArrayToElement(saveHere.m_K_start1,(unsigned char*)m_SK.k_start1().c_str(),false);

	//getting Kstart2:
	 mapper->byteArrayToElement(saveHere.m_K_start2,(unsigned char*)m_SK.k_start2().c_str(),false);

	 //getting Kt,i:
	 int numOfTrans = m_SK.k_t_1_size();  //get the number of Kt,i
	 for (int t=0; t<numOfTrans; t++)
	 {
		//getting Kt,1:
		 mapper->byteArrayToElement(saveHere.m_K_t[0][t],(unsigned char*)m_SK.k_t_1(t).c_str(),false);

		//getting Kt,2:
		 mapper->byteArrayToElement(saveHere.m_K_t[1][t],(unsigned char*)m_SK.k_t_2(t).c_str(),false);

		//getting Kt,3:
		 mapper->byteArrayToElement(saveHere.m_K_t[2][t],(unsigned char*)m_SK.k_t_3(t).c_str(),false);
	 }//for

	 //getting Kendx,i
	 int numOfAcceptanceStates = m_SK.k_for_q_x_1_size();
	 for (int x=0; x<numOfAcceptanceStates; x++)
	 {
		 //getting Kend,1:
		 mapper->byteArrayToElement(saveHere.m_K_for_q_x[0][x],(unsigned char*)m_SK.k_for_q_x_1(x).c_str(),false);

		 //getting Kend,2:
		 mapper->byteArrayToElement(saveHere.m_K_for_q_x[1][x],(unsigned char*)m_SK.k_for_q_x_2(x).c_str(),false);
	 }

}//end of deserializeSecretKey()

void ObjectSerializer::deserializeBond (EncryptionHandler::CT& saveHere, std::string bond_string)
{
	BilinearMappingHandler* mapper = m_encHandler->getBilinearMappingHandler(); //get the mapper
	m_Bond.ParseFromString(bond_string);  //deserialize the Bond

	//getting Cm:
	 mapper->byteArrayToElement(saveHere.m_Cm,(unsigned char*)m_Bond.cm().c_str(),true);

	//getting Cstart1:
	 mapper->byteArrayToElement(saveHere.m_C_start1,(unsigned char*)m_Bond.c_start1().c_str(),false);

	//getting Cstart2:
	 mapper->byteArrayToElement(saveHere.m_C_start2,(unsigned char*)m_Bond.c_start2().c_str(),false);

	//getting Cend1:
	 mapper->byteArrayToElement(saveHere.m_C_end1,(unsigned char*)m_Bond.c_end1().c_str(),false);

	//getting Cend2:
	 mapper->byteArrayToElement(saveHere.m_C_end2,(unsigned char*)m_Bond.c_end2().c_str(),false);

	 int numOfCi = m_Bond.c_i_1_size();   //get the number of Ci
	 for (int i=0; i<numOfCi ;i++)
	 {
		//getting Ci,1:
		 mapper->byteArrayToElement(saveHere.m_Ci[0][i],(unsigned char*)m_Bond.c_i_1(i).c_str(),false);

		 //for all the Ci,2 possibilities:
		 for (int j=1; j<ALPHABET_SIZE+1 ;j++)
		 {
			//getting Ci,2:
			 mapper->byteArrayToElement(saveHere.m_Ci[j][i],(unsigned char*)m_Bond.c_i_2(j-1).c_str(),false);
		 }
	 }//outer for
}//end of deserializeBond()

/*
 * Deserializes the SM string and returns the number of states.
 * IMPORTANT: SINCE THE RETRIEVING OF THE NUMBER OF STATES REQUIRES TO DE-SERIALIZE THE ENTIRE STRING,
 * WE SAVE THE DE-SERIALIZED RESULT FOR FURTHER USAGE.
 * @param SM_string - the serialized SM
 * @return - the number of states in the SM
 */
int ObjectSerializer::getNumOfStatesInStateMachineFromSerialized (std::string SM_string)
{
	m_Machine.ParseFromString(SM_string);  //deserialize the SM
	return m_Machine.statevec_size();  //returns the number of states
}//end of getNumOfStatesInStateMachineFromSerialized()

/*
 * Desrializes a stateMachine
 * @param saveHere - an empty stateMachine that can support the required number of states
 * @param SM_string - the stateMachine's serialized string
 */
void ObjectSerializer::deserializeStateMachine (StateMachine& saveHere, std::string SM_string)
{
	int transitionTable[ALPHABET_SIZE][2];
	int currentNumOfTrans;
	int numOfStates = m_Machine.statevec_size();

	if (numOfStates<1) //if the machine wasn't de-serialized by now
	{
		m_Machine.ParseFromString(SM_string);  //deserialize the SM
		numOfStates = m_Machine.statevec_size();
	}

	StateMachineAndKey::StateMachine_State serializerState;
	StateMachineAndKey::StateMachine_Transition serialzerTrans;

	//for every state:
	for (int i=0; i<numOfStates ;i++)
	{
		serializerState = m_Machine.statevec(i);  //get the current state
		currentNumOfTrans = serializerState.transitionvec_size();
		//for every non-trivial transition:
		for (int j=0; j<currentNumOfTrans ;j++)
		{
			serialzerTrans = serializerState.transitionvec(j);  	//get a transition
			transitionTable[j][0] = serialzerTrans.input().at(0); 	//get Sigma
			transitionTable[j][1] = serialzerTrans.nextstate(); 	//get the next state
		}//for every transition

		//add the constructed state to the SM:
		saveHere.addState(i,transitionTable,currentNumOfTrans,serializerState.isacceptancestate());
	}//for every state
}//end of deserializeStateMachine()

/*
 * Takes a valid SK and converts it to the protocol-buffer's SK format (that format will be later
 * used for serialization).
 * @param SK - the secret key
 * @param SM - reference to the stateMachine
 */
void ObjectSerializer::setSecretKey (EncryptionHandler::SK& SK, StateMachine& SM)
{
	BilinearMappingHandler* mapper = m_encHandler->getBilinearMappingHandler();  //get a ptr to a bilinear mapper
	int n;
	unsigned char data[MAX_ELEMENT_LENGTH_IN_BYTES];
	string *K_t_i, *K_end_i;


	//Kstart1:
	n = mapper->getElementLengthInBytes(SK.m_K_start1,false);  //get the num of bytes needed to represent Kstart1
	mapper->elementToByteArray(data,SK.m_K_start1,false);     //convert
	data[n]='\0';            //modify data to be a valid cstring

	m_SK.set_k_start1((char*)data);				//set Kstart1

	//Kstart2:
	n = mapper->getElementLengthInBytes(SK.m_K_start2,false);  //get the num of bytes needed to represent Kstart2
	mapper->elementToByteArray(data,SK.m_K_start2,false);     //convert
	data[n]='\0';            //modify data to be a valid cstring

	m_SK.set_k_start2((char*)data);				//set Kstart2

	//K_t_i  (3 elements for every transition)
	for (int t=0; t<SM.getTotalNumOfTransitions() ;t++)
	{
		//Kt,1:
		n = mapper->getElementLengthInBytes(SK.m_K_t[0][t],false);  //get the num of bytes needed to represent Kt,1
		mapper->elementToByteArray(data,SK.m_K_t[0][t],false);     //convert
		data[n]='\0';            //modify data to be a valid cstring

		K_t_i = m_SK.add_k_t_1();   //get a new, empty string for Kt,1
		K_t_i->assign((char*)data);				//set Kt,1

		//Kt,2:
		n = mapper->getElementLengthInBytes(SK.m_K_t[1][t],false);  //get the num of bytes needed to represent Kt,2
		mapper->elementToByteArray(data,SK.m_K_t[1][t],false);     //convert
		data[n]='\0';            //modify data to be a valid cstring

		K_t_i = m_SK.add_k_t_2();   //get a new, empty string for Kt,2
		K_t_i->assign((char*)data);				//set Kt,2

		//Kt,3:
		n = mapper->getElementLengthInBytes(SK.m_K_t[2][t],false);  //get the num of bytes needed to represent Kt,3
		mapper->elementToByteArray(data,SK.m_K_t[2][t],false);     //convert
		data[n]='\0';            //modify data to be a valid cstring

		K_t_i = m_SK.add_k_t_3();   //get a new, empty string for Kt,3
		K_t_i->assign((char*)data);				//set Kt,3
	}//finished with K_t_i

	//Kend,i (2 elements for every acceptance state)
	for (int x=0; x<SM.getTotalNumOfAcceptenceStates() ;x++)
	{
		//Kendx,1:
		n = mapper->getElementLengthInBytes(SK.m_K_for_q_x[0][x],false);  //get the num of bytes needed to represent Kendx,1
		mapper->elementToByteArray(data,SK.m_K_for_q_x[0][x],false);     //convert
		data[n]='\0';            //modify data to be a valid cstring

		K_end_i = m_SK.add_k_for_q_x_1();   //get a new, empty string for Kendx,1
		K_end_i->assign((char*)data);				//set Kendx,1

		//Kendx,2:
		n = mapper->getElementLengthInBytes(SK.m_K_for_q_x[1][x],false);  //get the num of bytes needed to represent Kendx,2
		mapper->elementToByteArray(data,SK.m_K_for_q_x[1][x],false);     //convert
		data[n]='\0';            //modify data to be a valid cstring

		K_end_i = m_SK.add_k_for_q_x_2();   //get a new, empty string for Kendx,2
		K_end_i->assign((char*)data);				//set Kendx,2

	}//finished with Kend,i

	m_isSecretKeySet=true;
}//end of setSecretKey()

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
	unsigned char data[MAX_ELEMENT_LENGTH_IN_BYTES];
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
	m_isBondSet = true;
}//end of setBond()
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

	m_isStateMachineSet = true;
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

/*
 * Returns a string that represents the StateMachine
 */
void ObjectSerializer::getSerializedStateMachineString(std::string saveHere)
{
	if (m_isStateMachineSet)
		m_Machine.SerializeToString(&saveHere);
	else
		saveHere.assign("");
}//end of getSerializedStateMachineString()

/*
 * Returns a string that represents the SecretKey
 */
void ObjectSerializer::getSerializedSecretKeyString(std::string saveHere)
{
	if (m_isSecretKeySet)
		m_SK.SerializeToString(&saveHere);
	else
		saveHere.assign("");
}//end of getSerializedSecretKeyString()

void ObjectSerializer::getSerializedBondString(std::string saveHere)
{
	if (m_isBondSet)
		m_Bond.SerializeToString(&saveHere);
	else
		saveHere.assign("");

}//end of getSerializedBondString()

ObjectSerializer::~ObjectSerializer()
{
	//Delete all global objects allocated by libprotobuf.
	 // google::protobuf::ShutdownProtobufLibrary();
}

