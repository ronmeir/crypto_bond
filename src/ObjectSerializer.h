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
#include "EncryptionHandler.h"
#include "StateMachineAndKey.pb.h"

class ObjectSerializer {
private:
	//members:
	//the following members are of classes that we're generated by the Protocol-Buffer
	StateMachineAndKey::Bond m_Bond;
	StateMachineAndKey::StateMachine m_Machine;
	StateMachineAndKey::SecretKey m_SK;
	EncryptionHandler* m_encHandler;

	//methods:
	void setSingleState(StateMachineAndKey::StateMachine_State* state,
								StateMachine& SM, const std::vector<Transition3Tuple>*,int);

public:
	ObjectSerializer(EncryptionHandler&);
	virtual ~ObjectSerializer();
	void setStateMachine (StateMachine& SM, std::string);
	void setSecretKey (EncryptionHandler::SK& SK);
	void setBond (EncryptionHandler::CT& CT);
	std::string getSerializedStateMachineString();
	std::string getSerializedSecretKeyString();
	std::string getSerializedBondString();
	int getNumOfStatesInStateMachineFromSerialized (std::string SM_string);
	void deserializeStateMachine (StateMachine& saveHere, std::string SM_string);
	void deserializeSecretKey (EncryptionHandler::SK& saveHere,std::string SK_string);
	void deserializeBond (EncryptionHandler::CT& saveHere, std::string bond_string);





};

#endif /* OBJECTSERIALIZER_H_ */
