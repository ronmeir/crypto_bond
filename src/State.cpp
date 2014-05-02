//   _____ _        _
//  / ____| |      | |
// | (___ | |_ __ _| |_ ___
//  \___ \| __/ _` | __/ _ \
//  ____) | || (_| | ||  __/
// |_____/ \__\__,_|\__\___|
//

/*
 * State.cpp
 * Describes a single state. Contains a description of all available transitions.
 */

#include "State.h"
#include <sstream>

using namespace std;
/**
 * Constructor for the inner class.
 * @param input - the input that triggers a transition
 * @param nextState - the next state the state machine should move to
 */
State::Transition::Transition(unsigned char input, int nextState)
{
	 Transition::mInput=input;
	 Transition::mNextStateID=nextState;
}//end of constructor

/*
 * Generates a 3 tuple for the current transition
 * @param currentStateID - the ID of the current state
 */
//TODO returning a non-primitive, should change it.
Transition3Tuple State::Transition::getTransition3Tuple (int currentStateID)
{
	Transition3Tuple ans(currentStateID,mNextStateID,mInput);
	return ans;
}//end of getTransition3Tuple()

/*
 * Constructor.
 * @param stateID - The unique ID that will define this state. THIS VALUE MUST BE IN THE RANGE [0...NUM_OF_STATES-1]
 * @param transitionTable - An Int table of size NUM_OF_STATES x 2, when the 1 column in each row
 * contains the input value and the 2nd column contains the next state ID for that input
 * @param numOfRows - the number of transitions given in the table
 * @param isAcceptanceState - defines whether this state is an acceptance state or not
 */
State::State(int stateID, int transitionTable[][2], int numOfRows, bool isAcceptanceState)
{
	mStateID=stateID;
	mIsAcceptanceState = isAcceptanceState;

	//init the transition table pointers
	for (int i=0; i<ALPHABET_SIZE ;i++)
		mTransitionTable[i]=NULL;

    updateTransitionTable(transitionTable, numOfRows); //translate the user's format to our format
}//end of constructor

/**
 * Translates the user-given transition table to our local format
 * @param transitionTable - An Int table of size NUM_OF_STATES x 2, when the 1 column in each row
 * @param numOfRows - the number of transitions given in the table
 */
void State::updateTransitionTable(int userFormatTable[][2], int numOfRows)
{
	int i;
	Transition* transition; //a ptr to a single transition
	for (i=0; i<numOfRows ;i++) //for every transition given by the user
	{
		transition = new Transition(userFormatTable[i][0],userFormatTable[i][1]); //create a new transition
		mTransitionTable[transition->mInput] = transition;  //save the new transition in the state's transition table
		//At the state's level, sigma allows a direct access to the transition
	}

	//creating all possible undefined transitions:
	for (i=0; i<ALPHABET_SIZE ;i++)
	{
		if (mTransitionTable[i]==NULL) //if this transition wasn't defined by the user
		{
			//set a default transition to self:
			transition = new Transition(i,mStateID); //create a new transition
			mTransitionTable[i] = transition;  //save the new transition in the state's transition table
		}
	}//for

}//end of updateTransitionTable()

/*
 * Returns the state ID
 */
int State::getStateID()
{
	return mStateID;
}//end of getStateID()

/*
 * Returns the next state the machine should transit to.
 * @param input - the current input to the state machine
 * @return - the ID of the next state if it exists, -1 otherwise.
 */
int State::getNextStateID(unsigned char input)
{
	int nextState = -1;
	if (mTransitionTable[input]!=NULL) //if there is a valid transition for this input
	{
		nextState = mTransitionTable[input]->mNextStateID;  //get the next state
	}

	return nextState;
}//end of getNextState()

/*
 * Returns whether this state is an acceptance state or not.
 */
bool State::getisAcceptanceState()
{
	return mIsAcceptanceState;
}//end of getisAcceptanceState()

/*
 * Creates transition 3-tuples for all transitions in this state.
 * Saves the answer in ans
 * @param ans - where the 3-tuples will be saved at
 */
void State::translateStateTo3Tuples(std::vector<Transition3Tuple>* ans)
{
	for (int i=0; i < ALPHABET_SIZE ;i++) //for every possible transition
	{
		ans->push_back(mTransitionTable[i]->getTransition3Tuple(mStateID)); //get a single transition 3-tuple
	}
}//end of translateStateTo3Tuples()


//Returns a string representation of the state
std::string State::toString() const
{
	ostringstream convert;   // stream used for the conversion
	convert << "State #" << mStateID << ". Acceptance status: " << (mIsAcceptanceState? "True! ZE VIRUS!!!" : "False");
	return convert.str();
}//end of toString()

/*
 * Destructor
 */
State::~State()
{
	//Delete all allocated transitions:
	for (int i=0; i<ALPHABET_SIZE ;i++)
	{
		if (mTransitionTable[i]!=NULL)
			delete(mTransitionTable[i]);
	}
}//end of destructor


