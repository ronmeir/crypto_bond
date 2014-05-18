//   _____ _        _
//  / ____| |      | |
// | (___ | |_ __ _| |_ ___
//  \___ \| __/ _` | __/ _ \
//  ____) | || (_| | ||  __/
// |_____/ \__\__,_|\__\___|
//

/*
 * State.h
 * Defines a single state in a state machine.
 * Contains a nested class named 'Transition' that represents a single possible transition from this state.
 * The state's main members are:
 * 		ID - a unique ID
 * 		isAcceptanceState - indicates if we've detected a virus or not.
 * 		Transition table - an array of 'Transition's, of size ALPHABET_SIZE. Implemented to achieve an O(1) lookup time.
 * 						   Not all entries are valid, only the ones associated with valid transitions.
 */
#include <string>
#include <vector>
#include "Transition3Tuple.h"
#include "Constants.h"

#ifndef STATE_H_
#define STATE_H_

//#define ALPHABET_SIZE 256

class State {

private:

	//The next class defines a single transition from the current state
	class Transition
	{
	public:
		 unsigned char mInput;  //our alphabet is 8-bit symbols
		 int mNextStateID;

		 //Constructor:
		 Transition(unsigned char input, int nextState);
		 Transition3Tuple getTransition3Tuple (int currentStateID);

	};//end of class

	//Variables:
	int mStateID; //the state's unique ID
	bool mIsAcceptanceState;
	Transition* mTransitionTable[ALPHABET_SIZE]; //the state transition table

	//Functions:
	void updateTransitionTable(int userFormatTable[][2], int numOfRows);

public:

	State(int stateID, int transitionTable[][2], int numOfRows, bool IsAcceptanceState);
	virtual ~State();
	int getStateID();
	int getNextStateID (unsigned char input);
	bool getIsAcceptanceState();
	void translateStateTo3Tuples(std::vector<Transition3Tuple>* ans);
	virtual std::string toString() const;
};//end of class

#endif /* STATE_H_ */
