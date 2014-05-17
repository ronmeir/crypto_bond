/*
 * ObjectSerializer.h
 *
 *  Created on: May 6, 2014
 *      Author: ron
 */

#ifndef OBJECTSERIALIZER_H_
#define OBJECTSERIALIZER_H_
#include <vector>
#include "Transition3Tuple.h"
#include "StateMachine.h"
#include "StateMachineAndKey.pb.h"

class ObjectSerializer {
private:
	//members:
	StateMachineAndKey::Bond m_Bond;
	StateMachineAndKey::StateMachine m_Machine;
	StateMachineAndKey::SecretKey m_SK;

	void getRelevantTrans(StateMachine& SM , std::vector<Transition3Tuple>& trans);

public:
	ObjectSerializer();
	virtual ~ObjectSerializer();





};

#endif /* OBJECTSERIALIZER_H_ */
