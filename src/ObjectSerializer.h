/*
 * ObjectSerializer.h
 *
 *  Created on: May 6, 2014
 *      Author: ron
 */

#ifndef OBJECTSERIALIZER_H_
#define OBJECTSERIALIZER_H_
#include <vector>
#include <string>
#include "Transition3Tuple.h"
#include "StateMachine.h"
#include "StateMachineAndKey.pb.h"

class ObjectSerializer {
private:
	//members:
	StateMachineAndKey::Bond m_Bond;
	StateMachineAndKey::StateMachine m_Machine;
	StateMachineAndKey::SecretKey m_SK;

	//methods:
	void getRelevantTrans(StateMachine& SM , std::vector<Transition3Tuple>& trans);

public:
	ObjectSerializer();
	virtual ~ObjectSerializer();
	const void setStateMachine (const StateMachine& SM);
	const void setSecretKey (const EncryptionHandler::SK& SK);
	const void setBond (const EncryptionHandler::CT& CT);
	std::string getSerializedStateMachineString();
	std::string getSerializedSecretKeyString();
	std::string getSerializedBondString();





};

#endif /* OBJECTSERIALIZER_H_ */
