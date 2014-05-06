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
class ObjectSerializer {
public:
	ObjectSerializer();
	virtual ~ObjectSerializer();
	void getRelevantTrans(StateMachine& SM , std::vector<Transition3Tuple>& trans);




};

#endif /* OBJECTSERIALIZER_H_ */
