//   _____ _        _       __  __            _     _
//  / ____| |      | |     |  \/  |          | |   (_)
// | (___ | |_ __ _| |_ ___| \  / | __ _  ___| |__  _ _ __   ___
//  \___ \| __/ _` | __/ _ \ |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  ____) | || (_| | ||  __/ |  | | (_| | (__| | | | | | | |  __/
// |_____/ \__\__,_|\__\___|_|  |_|\__,_|\___|_| |_|_|_| |_|\___|
//

/*
 * StateMachine.h
 * Defines a finite state machine.
 */

#include "State.h"
#include "Constants.h"
#include <string>
#include <vector>

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_


class StateMachine
{
private:
	State** mStates;								  //the entire state set
	int mInitialState;						  		  //the ID of the machine's initial state (q0)
	int mCurrentStateID;							  //the id of the current state
	int mTotalNumOfStates;							  //the total number of different states
	std::vector<Transition3Tuple>  mTransitionTuples; //a set of all the transition triples: <x,y,sigma>
	std::vector<int> mIDsOfAcceptanceStates;

public:
	StateMachine(int numOfStates, int initialStateID);
	virtual ~StateMachine();

	int addState (int stateID, int transitionTable[][2], int numOfRows, bool IsAcceptanceState);
	int moveToNextState (unsigned char input);
    const int getCurrentStateID();
	const bool getisAcceptanceState();
	virtual std::string toString() const;
	const std::vector<Transition3Tuple>* translateStateMachineToTriples();
	const int getTotalNumOfStates();
	const std::vector<int>*  getAcceptanceStates();
	int getIndexOfAcceptanceStateInTheAcceptanceStatesVector(int stateID);
	const int getTotalNumOfTransitions();
	const int getTotalNumOfAcceptenceStates();
	void resetMachineToInitialState();

};

#endif /* STATEMACHINE_H_ */
