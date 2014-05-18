/*
 * ObjectSerializer.cpp
 * Responsible for serializing and de-serializing the StateMachine, SecretKey and CipherText objects so
 * they can be easily sent and received via socket.
 */

#include "ObjectSerializer.h"

using namespace std;

ObjectSerializer::ObjectSerializer()
{
	//no need for initialization. the member objects are already initialized.

  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
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

void ObjectSerializer::setSingleState(StateMachineAndKey::StateMachine_State* saveHere, StateMachine& SM,
										const vector<Transition3Tuple>*  allTransitions, int stateID)
{
	//check if the current state is an acceptance state:
	SM.getIndexOfAcceptanceStateInTheAcceptanceStatesVector(stateID)==-1?
			saveHere->set_isacceptancestate(false) : saveHere->set_isacceptancestate(true);

	//setting all non-trivial transitions (X,Y,Sigma):

	//set the stateID (X):
	saveHere->set_stateid(stateID);

	int currentIndexInTransVector = stateID*ALPHABET_SIZE;
	for (int j=0 ; j<256 ;j++)
	{
		Transition3Tuple trans3tuple = allTransitions->at(currentIndexInTransVector); //get a transition
		if (trans3tuple.get_Y()!=stateID)  //if this is a non-trivial transition (not a self loop)
		{
			StateMachineAndKey::StateMachine_Transition* transInSerializer =
					saveHere->add_transitionvec();         //get a new, empty transition
			transInSerializer->set_input( string((char)trans3tuple.get_Sigma()) ); //set sigma
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

