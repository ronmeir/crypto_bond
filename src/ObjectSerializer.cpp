/*
 * ObjectSerializer.cpp
 *
 *  Created on: May 6, 2014
 *      Author: ron
 */

#include "ObjectSerializer.h"
ObjectSerializer::ObjectSerializer()
{
	//no need for initialization. the member objects are already initialized.
}

ObjectSerializer::~ObjectSerializer()
{
	// TODO Auto-generated destructor stub
}


void ObjectSerializer::getRelevantTrans(StateMachine& SM , std::vector<Transition3Tuple>& trans){

	const std::vector<Transition3Tuple>* t =SM.translateStateMachineToTriples(); //get all the trans of the state machine in a vector
	int size =SM.getTotalNumOfStates();
	for(int i=0;i<size;i++){
		int ctr=0; //count the #trans. from the i'th state

		//this is a counting "for"
		for(int j=0;i<ALPHABET_SIZE;j++){
			Transition3Tuple tran =t->at(i*ALPHABET_SIZE+j);

			//if a real trans. happens here we want to count it!
			if(tran.get_X() != tran.get_Y()){
				ctr++;
				//TODO create msgTransObject and push it to "trans"
			}//if
		}//for (j)
		//done with counting for the i's state s[i]

		//TODO push the ctr into the "amountOfTransPerState"
	}//for (i)

//TODO compile the stupid "proto" and check ALL the functions!!!!!!

}//end of getRelevantTrans
