/*
   ____  _     _           _    _____           _       _ _
  / __ \| |   (_)         | |  / ____|         (_)     | (_)
 | |  | | |__  _  ___  ___| |_| (___   ___ _ __ _  __ _| |_ _______ _ __
 | |  | | '_ \| |/ _ \/ __| __|\___ \ / _ \ '__| |/ _` | | |_  / _ \ '__|
 | |__| | |_) | |  __/ (__| |_ ____) |  __/ |  | | (_| | | |/ /  __/ |
  \____/|_.__/| |\___|\___|\__|_____/ \___|_|  |_|\__,_|_|_/___\___|_|
             _/ |
            |__/

 * Responsible for serializing and de-serializing the StateMachine, SecretKey and CipherText objects so
 * they can be easily sent and received via socket.
 */

#include "ObjectSerializer.h"

using namespace std;

//Constructor
ObjectSerializer::ObjectSerializer(BilinearMappingHandler& mapper)
{
	m_mapper = &mapper;   //set a ptr to an encryption handler.
	m_isBondSet = false;
	m_isSecretKeySet = false;
	m_isStateMachineSet = false;
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against:
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}//end of constructor

/*
 * Used by the client for clearing the bond in PT before sending the entire bond CT object to the server
 */
void ObjectSerializer::clearBondInPlainText ()
{
	if (m_Bond.has_bond_in_pt_compressed())
		m_Bond.clear_bond_in_pt_compressed();
}//end of clearBondInPlainText()

bool ObjectSerializer::hasBondInPlainText()
{
	return m_Bond.has_bond_in_pt_compressed();
}//end of hasBondInPlainText()

/*
 * Used by the CA to get the bond in PT
 * @param saveHere - the decompressed element (the PT-bond) will be saved here
 * @param isMemberOfGT - always true in our crypto scheme
 */
bool ObjectSerializer::getBondInPlainText (memberElement& saveHere, bool isMemberOfGT)
{
	if (!m_Bond.has_bond_in_pt_compressed()) //if there's no PT-bond set
		return false;

	m_mapper->byteArrayToElement(saveHere,(unsigned char*)m_Bond.bond_in_pt_compressed().c_str(),isMemberOfGT);
	return true;
}//end of getBondInPlainText()

/*
 * Used by the client to add the secret in PT to the bond object before sending it to the CA.
 * @param bond - the secret
 * @param isMemberOfGT - always true in our crypto scheme
 */
void ObjectSerializer::setBondInPlainText (memberElement& bond, bool isMemberOfGT)
{
	if (m_isBondSet)
	{
		int bondPT_size = m_mapper->getElementLengthInBytes(bond,isMemberOfGT);
		unsigned char compressedBond[bondPT_size+1];  //allocate a buffer

		m_mapper->elementToByteArray(compressedBond,bond,isMemberOfGT); //get the compresses element

		m_Bond.set_bond_in_pt_compressed((char*)compressedBond);  //set
	}
}//end of setBondInPlainText()

/*
 * Deserializes the SK string
 * @param saveHere - This SK should only contain allocated space that can be filled.
 * @param SK_string - the serialized SK
 */
void ObjectSerializer::deserializeSecretKey (EncryptionHandler::SK& saveHere,std::string SK_string)
{

	m_SK.ParseFromString(SK_string);  //deserialize the SK

	//getting Kstart1:
	 m_mapper->byteArrayToElement(saveHere.m_K_start1,(unsigned char*)m_SK.k_start1().c_str(),false);

	//getting Kstart2:
	 m_mapper->byteArrayToElement(saveHere.m_K_start2,(unsigned char*)m_SK.k_start2().c_str(),false);

	 //getting Kt,i:
	 int numOfTrans = m_SK.k_t_1_size();  //get the number of Kt,i
	 for (int t=0; t<numOfTrans; t++)
	 {
		//getting Kt,1:
		 m_mapper->byteArrayToElement(saveHere.m_K_t[0][t],(unsigned char*)m_SK.k_t_1(t).c_str(),false);

		//getting Kt,2:
		 m_mapper->byteArrayToElement(saveHere.m_K_t[1][t],(unsigned char*)m_SK.k_t_2(t).c_str(),false);

		//getting Kt,3:
		 m_mapper->byteArrayToElement(saveHere.m_K_t[2][t],(unsigned char*)m_SK.k_t_3(t).c_str(),false);
	 }//for

	 //getting Kendx,i
	 int numOfAcceptanceStates = m_SK.k_for_q_x_1_size();
	 for (int x=0; x<numOfAcceptanceStates; x++)
	 {
		 //getting Kend,1:
		 m_mapper->byteArrayToElement(saveHere.m_K_for_q_x[0][x],(unsigned char*)m_SK.k_for_q_x_1(x).c_str(),false);

		 //getting Kend,2:
		 m_mapper->byteArrayToElement(saveHere.m_K_for_q_x[1][x],(unsigned char*)m_SK.k_for_q_x_2(x).c_str(),false);
	 }

}//end of deserializeSecretKey()

void ObjectSerializer::deserializeBond (EncryptionHandler::CT& saveHere, std::string bond_string)
{
	m_Bond.ParseFromString(bond_string);  //deserialize the Bond

	//getting Cm:
	m_mapper->byteArrayToElement(saveHere.m_Cm,(unsigned char*)m_Bond.cm().c_str(),true);

	//getting Cstart1:
	m_mapper->byteArrayToElement(saveHere.m_C_start1,(unsigned char*)m_Bond.c_start1().c_str(),false);

	//getting Cstart2:
	m_mapper->byteArrayToElement(saveHere.m_C_start2,(unsigned char*)m_Bond.c_start2().c_str(),false);

	//getting Cend1:
	m_mapper->byteArrayToElement(saveHere.m_C_end1,(unsigned char*)m_Bond.c_end1().c_str(),false);

	//getting Cend2:
	m_mapper->byteArrayToElement(saveHere.m_C_end2,(unsigned char*)m_Bond.c_end2().c_str(),false);

	 int indexInC_i_2_array = 0; //will run from 0 to MAX_MSG_LENGTH*ALPHABET_SIZE
	 int numOfCi = m_Bond.c_i_1_size();   //get the number of Ci
	 for (int i=0; i<numOfCi ;i++)
	 {

		//getting Ci,1:
		 m_mapper->byteArrayToElement(saveHere.m_Ci[0][i],(unsigned char*)m_Bond.c_i_1(i).c_str(),false);

		 //for all the Ci,2 possibilities:
		 for (int j=1; j<ALPHABET_SIZE+1 ;j++)
		 {
			//getting Ci,2:
			 m_mapper->byteArrayToElement(saveHere.m_Ci[j][i],
					 (unsigned char*)m_Bond.c_i_2(indexInC_i_2_array).c_str(),false);
			 indexInC_i_2_array++; //move to the next c_i_2
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
void ObjectSerializer::deserializeStateMachine (StateMachine& saveHere,string* saveVirusHere, std::string SM_string)
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

	*saveVirusHere = m_Machine.virus();   //extract the virus string.

	//for every state:
	for (int i=0; i<numOfStates ;i++)
	{
		serializerState = m_Machine.statevec(i);  //get the current state
		currentNumOfTrans = serializerState.transitionvec_size();
		//for every non-trivial transition:
		for (int j=0; j<currentNumOfTrans ;j++)
		{
			serialzerTrans = serializerState.transitionvec(j);  	//get a transition
			transitionTable[j][0] = serialzerTrans.input(); 	//get Sigma
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
	int n;
	unsigned char data[MAX_ELEMENT_LENGTH_IN_BYTES];
	string *K_t_i, *K_end_i;


	//Kstart1:
	n = m_mapper->getElementLengthInBytes(SK.m_K_start1,false);  //get the num of bytes needed to represent Kstart1
	m_mapper->elementToByteArray(data,SK.m_K_start1,false);     //convert

	m_SK.set_k_start1((char*)data,n);				//set Kstart1

	//Kstart2:
	n = m_mapper->getElementLengthInBytes(SK.m_K_start2,false);  //get the num of bytes needed to represent Kstart2
	m_mapper->elementToByteArray(data,SK.m_K_start2,false);     //convert

	m_SK.set_k_start2((char*)data,n);				//set Kstart2

	//K_t_i  (3 elements for every transition)
	for (int t=0; t<SM.getTotalNumOfTransitions() ;t++)
	{
		//Kt,1:
		n = m_mapper->getElementLengthInBytes(SK.m_K_t[0][t],false);  //get the num of bytes needed to represent Kt,1
		m_mapper->elementToByteArray(data,SK.m_K_t[0][t],false);     //convert

		K_t_i = m_SK.add_k_t_1();   //get a new, empty string for Kt,1
		K_t_i->assign((char*)data,n);				//set Kt,1

		//Kt,2:
		n = m_mapper->getElementLengthInBytes(SK.m_K_t[1][t],false);  //get the num of bytes needed to represent Kt,2
		m_mapper->elementToByteArray(data,SK.m_K_t[1][t],false);     //convert

		K_t_i = m_SK.add_k_t_2();   //get a new, empty string for Kt,2
		K_t_i->assign((char*)data,n);				//set Kt,2

		//Kt,3:
		n = m_mapper->getElementLengthInBytes(SK.m_K_t[2][t],false);  //get the num of bytes needed to represent Kt,3
		m_mapper->elementToByteArray(data,SK.m_K_t[2][t],false);     //convert

		K_t_i = m_SK.add_k_t_3();   //get a new, empty string for Kt,3
		K_t_i->assign((char*)data,n);				//set Kt,3
	}//finished with K_t_i

	//Kend,i (2 elements for every acceptance state)
	for (int x=0; x<SM.getTotalNumOfAcceptenceStates() ;x++)
	{
		//Kendx,1:
		n = m_mapper->getElementLengthInBytes(SK.m_K_for_q_x[0][x],false);  //get the num of bytes needed to represent Kendx,1
		m_mapper->elementToByteArray(data,SK.m_K_for_q_x[0][x],false);     //convert

		K_end_i = m_SK.add_k_for_q_x_1();   //get a new, empty string for Kendx,1
		K_end_i->assign((char*)data,n);				//set Kendx,1

		//Kendx,2:
		n = m_mapper->getElementLengthInBytes(SK.m_K_for_q_x[1][x],false);  //get the num of bytes needed to represent Kendx,2
		m_mapper->elementToByteArray(data,SK.m_K_for_q_x[1][x],false);     //convert

		K_end_i = m_SK.add_k_for_q_x_2();   //get a new, empty string for Kendx,2
		K_end_i->assign((char*)data,n);				//set Kendx,2

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
	int n;
	unsigned char data[MAX_ELEMENT_LENGTH_IN_BYTES];
	string *C_i_1, *C_i_2;

	//Cm:
	n = m_mapper->getElementLengthInBytes(CT.m_Cm,true);  //get the num of bytes needed to represent Cm
	m_mapper->elementToByteArray(data,CT.m_Cm,true);     //convert

	m_Bond.set_cm((char*)data,n);				//set Cm

	//Cstart1:
	n = m_mapper->getElementLengthInBytes(CT.m_C_start1,false);  //get the num of bytes needed to represent Cstart1
	m_mapper->elementToByteArray(data,CT.m_C_start1,false);     //convert

	m_Bond.set_c_start1((char*)data,n);				//set Cstart1

	//Cstart2:
	n = m_mapper->getElementLengthInBytes(CT.m_C_start2,false);  //get the num of bytes needed to represent Cstart2
	m_mapper->elementToByteArray(data,CT.m_C_start2,false);     //convert

	m_Bond.set_c_start2((char*)data,n);				//set Cstart2

	//Cend1:
	n = m_mapper->getElementLengthInBytes(CT.m_C_end1,false);  //get the num of bytes needed to represent Cend1
	m_mapper->elementToByteArray(data,CT.m_C_end1,false);     //convert

	m_Bond.set_c_end1((char*)data,n);				//set Cend1

	//Cend2:
	n = m_mapper->getElementLengthInBytes(CT.m_C_end2,false);  //get the num of bytes needed to represent Cend2
	m_mapper->elementToByteArray(data,CT.m_C_end2,false);     //convert

	m_Bond.set_c_end2((char*)data,n);				//set Cend2

	//Ci,1 and Ci,2:
	//i is the current column
	for (int i=0; i<g_maxMessageLength ;i++)
	{
		//adding Ci,1:
		n = m_mapper->getElementLengthInBytes(CT.m_Ci[0][i],false);  //get the num of bytes needed to represent Ci,1
		m_mapper->elementToByteArray(data,CT.m_Ci[0][i],false);     //convert

		C_i_1 = m_Bond.add_c_i_1();      //get a new empty string
		C_i_1->assign((char*)data,n);				//set Ci,1

		//j is the current row
		for (int j=1; j<ALPHABET_SIZE+1 ;j++)
		{
			//adding all possible Ci,2:
			n = m_mapper->getElementLengthInBytes(CT.m_Ci[j][i],false);  //get the num of bytes needed to represent Ci,2
			m_mapper->elementToByteArray(data,CT.m_Ci[j][i],false);     //convert

			C_i_2 = m_Bond.add_c_i_2();      //get a new empty string
			C_i_2->assign((char*)data,n);				//set Ci,2

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
			transInSerializer->set_input( trans3tuple.get_Sigma() ); //set sigma
			transInSerializer->set_nextstate(trans3tuple.get_Y());  //set Y
		}
		currentIndexInTransVector++; //advance
	}//for
}//end of setSingleState()

/*
 * Returns a string that represents the StateMachine
 */
void ObjectSerializer::getSerializedStateMachineString(std::string& saveHere)
{
	if (m_isStateMachineSet)
		m_Machine.SerializeToString(&saveHere);
	else
		saveHere.assign("");
}//end of getSerializedStateMachineString()

/*
 * Returns a string that represents the SecretKey
 */
void ObjectSerializer::getSerializedSecretKeyString(std::string& saveHere)
{
	if (m_isSecretKeySet)
		m_SK.SerializeToString(&saveHere);
	else
		saveHere.assign("");
}//end of getSerializedSecretKeyString()

/*
 * Returns a string that represents the Bond
 */
void ObjectSerializer::getSerializedBondString(std::string& saveHere)
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

