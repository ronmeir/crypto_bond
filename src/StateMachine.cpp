//   _____ _        _       __  __            _     _
//  / ____| |      | |     |  \/  |          | |   (_)
// | (___ | |_ __ _| |_ ___| \  / | __ _  ___| |__  _ _ __   ___
//  \___ \| __/ _` | __/ _ \ |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  ____) | || (_| | ||  __/ |  | | (_| | (__| | | | | | | |  __/
// |_____/ \__\__,_|\__\___|_|  |_|\__,_|\___|_| |_|_|_| |_|\___|
//

/*
 * StateMachine.cpp
 * Describes a finite state machine.
 */

#include "StateMachine.h"
#include <iostream>

using namespace std;

/*
 * Constructor. Initializes an empty state machine
 */
StateMachine::StateMachine(int numOfStates, int initialStateID)
{

	mCurrentStateID=initialStateID;
	mInitialState = initialStateID;

	mTotalNumOfStates=numOfStates;

	mStates = new State* [numOfStates]; //allocate an array of pointers to states

	for (int i=0; i<numOfStates ;i++) //init all entries
		mStates[i]=NULL;

}//end of constructor

/*
 * Adds a single state to the state machine.
 * @param stateID - The unique ID that will define this state. THIS VALUE MUST BE IN THE RANGE [0...NUM_OF_STATES-1]
 * @param transitionTable - An Int table of size NUM_OF_STATES x 2, when the 1st column in each row
 * contains the input value and the 2nd column contains the next state ID for that input
 * @param numOfRows - the number of transitions given in the table
 * @param isAcceptanceState - defines whether this state is an acceptance state or not
 * @return 0 if the state was successfully added, -1 otherwise.
 */
int StateMachine::addState (int stateID, int transitionTable[][2], int numOfRows, bool IsAcceptanceState)
{
	if (stateID<0 || stateID >= mTotalNumOfStates ) //if the state ID is invalid
		{
		cout << "All state IDs must be in the range [0,Num_Of_States-1]";
		return -1;
		}
	mStates[stateID] = new State(stateID,transitionTable,numOfRows,IsAcceptanceState); //create a new state
	//At the state-machine level, x allows a direct access to the state's transition block [ALPHABET_SIZE entries per state]

	//checking if this is an acceptance state:
	if (IsAcceptanceState)
		mIDsOfAcceptanceStates.push_back(stateID);  //save this state's ID

	return 0;
}//end of addState()

/**
 * Runs a string through the SM, WITHOUT CHANGING THE SM'S STATE
 * @param str - the string to look for viruses in
 * @return bool - true if the machine has reached an acceptance state, false otherwise.
 */
bool StateMachine::checkStringForViruses (std::string& str)
{
	int current_state = mInitialState;

	//handling the case of an uninitialized state:
	if (mStates[mInitialState]==NULL)
	{
		cout << "Error! Can't move, initial state is undefined!";
		return -1;
	}

	for (int i=0; i<str.length() ;i++) //running on all the string's chars
	{
		//state is defined. retrieving the next state:
		current_state = mStates[current_state]->getNextStateID(str.at(i));
		if (current_state==-1) //if the machine can move to a new state
		{
			cout << "The current state has no defined transition for the given input!\n";
			return false;
		}
	}//for

	return mStates[current_state]->getIsAcceptanceState();

}//end of checkStringForViruses()

/**
 * IMPORTANT: NOT THREAD SAFE. THIS FUNCTION CHANGES THE MACHINE'S CURRENT STATE
 * Tries to move the machine to the next state
 * @param input - the given input
 * @return 0 if the machine has successfully moved to the next state, -1 otherwise
 */
int StateMachine::moveToNextState (unsigned char input)
{
	int nextStateID;

	//handling the case of an uninitialized state:
	if (mStates[mCurrentStateID]==NULL)
	{
		cout << "Error! Can't move, current state is undefined!";
		return -1;
	}

	//state is defined. retrieving the next state:
	nextStateID = mStates[mCurrentStateID]->getNextStateID(input);
	if (nextStateID!=-1) //if the machine can move to a new state
	{
		mCurrentStateID=nextStateID; //update the current state
		return 0;
	}
	else
	{
		cout << "The current state has no defined transition for the given input!\n";
		return -1;
	}
}//end of moveToNextState()

/*
 * Returns the ID of the current state
 */
const int StateMachine::getCurrentStateID()
{
	return mCurrentStateID;
}//end of getCurrentStateID()

/*
 * Returns whether the machine has reached an acceptance state.
 */
const bool StateMachine::getIsAcceptanceState()
{
	return mStates[mCurrentStateID]->getIsAcceptanceState();
}//end of getisAcceptanceState()

/*
 * Creates a full vector with all available transitions, sorted in 2 levels:
 * The first level contains states sorted by state_ID.
 * IMPORTANT: THE TRANSITIONS VECTOR IS INVALID PRIOR THIS FUNCTION BEING CALLED!!
 * The second level contains transitions sorted by input (sigma).
 * For example, the location of the transition <x,y,sigma> is located at index [x*ALPHABET_SIZE + sigma]
 * Returns a ptr to an std::vector containing all the transition 3-tuples
 */
 const std::vector<Transition3Tuple>* StateMachine::translateStateMachineToTriples()
{
		for (int i = 0; i < mTotalNumOfStates; i++)
		{
			//updating the state 3-tuple vector:
			if (mStates[i] != NULL)
				mStates[i]->translateStateTo3Tuples(&mTransitionTuples);
		}
	return &mTransitionTuples;
}//end of translateStateMachineToTriples()

 /*
  * Returns the number of states in the machine
  */
const int StateMachine::getTotalNumOfStates()
{
	return mTotalNumOfStates;
}//end of getTotalNumOfStates()

/*
 * Returns the total number of transitions in the state machine.
 */
const int StateMachine::getTotalNumOfTransitions(){

	 return mTotalNumOfStates*ALPHABET_SIZE;
}//end of getTotalNumOfTransition()

/*
 * Returns the total number of acceptance states in the state machine.
 */
const int StateMachine::getTotalNumOfAcceptenceStates(){
	return mIDsOfAcceptanceStates.size();
}//end of getTotalNumOfAcceptenceStates()

/*
 * Returns a vector with all acceptance state IDs
 */
const vector<int>* StateMachine::getAcceptanceStates()
{
	return &mIDsOfAcceptanceStates;
}//end of getAcceptanceStates()

/*
 * Returns the index of a state in the acceptance state vector
 * @return the state's index at the vector, -1 if it doesn't exist
 */
int StateMachine::getIndexOfAcceptanceStateInTheAcceptanceStatesVector(int stateID)
{
	int ans=-1;
	//run over the entire vector
	for (int i=0; i< mIDsOfAcceptanceStates.size() ;i++)
	{
		if (mIDsOfAcceptanceStates.at(i)==stateID)
			ans=i;
	}

	return ans;
}//end of getIndexOfAcceptanceStateInTheAcceptanceStatesVector()

//Resets the machine to q0 (the initial state)
void StateMachine::resetMachineToInitialState()
{
	mCurrentStateID=mInitialState;
}//end of resetMachineToInitialState()

//Returns a string representation of the current state
std::string StateMachine::toString() const
{
	State* currentState = mStates[mCurrentStateID];
	std::string res;

	//if the machine is at a valid state
	if (currentState!=NULL)
	{
		res.append("Current state is: ");
		res += currentState->toString();
	}
	else
	{
		res.append("The machine is currently at an invalid state!");
	}

	return res;
}//end of toString()

/*
 * Destructor
 */
StateMachine::~StateMachine()
{
	//delete all states:
	for (int i=0; i<mTotalNumOfStates ;i++)
	{
		if (mStates[i]!=NULL)
			delete(mStates[i]);
	}

	delete[] mStates;  //delete the ptr array

}//Destructor

