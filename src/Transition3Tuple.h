//  _______                  _ _   _             ____ _______          _
// |__   __|                (_) | (_)           |___ \__   __|        | |
//    | |_ __ __ _ _ __  ___ _| |_ _  ___  _ __   __) | | |_   _ _ __ | | ___
//    | | '__/ _` | '_ \/ __| | __| |/ _ \| '_ \ |__ <  | | | | | '_ \| |/ _ \
//    | | | | (_| | | | \__ \ | |_| | (_) | | | |___) | | | |_| | |_) | |  __/
//    |_|_|  \__,_|_| |_|___/_|\__|_|\___/|_| |_|____/  |_|\__,_| .__/|_|\___|
//                                                              | |
//                                                              |_|

/*
 * Transition3Tuple.h
 */
#include <string>

#ifndef TRANSITION3TUPLE_H_
#define TRANSITION3TUPLE_H_

class Transition3Tuple
{
private:
	int m_x,m_y; 			  //x is the current state, y is the next state
	unsigned char m_sigma;  //sigma is the input that causes a transition from x to y
	int m_transitionID;       //a unique ID


public:
	Transition3Tuple(int x,int y, unsigned char sigma);
	virtual ~Transition3Tuple();
	int set_X(int x);
	int set_Y(int y);
	int set_Sigma(unsigned char sigma);
	int get_X();
	int get_Y();
	unsigned char get_Sigma();
	std::string toString() const;

};

#endif /* TRANSITION3TUPLE_H_ */
