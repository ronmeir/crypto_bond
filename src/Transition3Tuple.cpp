//  _______                  _ _   _             ____ _______          _
// |__   __|                (_) | (_)           |___ \__   __|        | |
//    | |_ __ __ _ _ __  ___ _| |_ _  ___  _ __   __) | | |_   _ _ __ | | ___
//    | | '__/ _` | '_ \/ __| | __| |/ _ \| '_ \ |__ <  | | | | | '_ \| |/ _ \
//    | | | | (_| | | | \__ \ | |_| | (_) | | | |___) | | | |_| | |_) | |  __/
//    |_|_|  \__,_|_| |_|___/_|\__|_|\___/|_| |_|____/  |_|\__,_| .__/|_|\___|
//                                                              | |
//                                                              |_|

/*
 * Transition3Tuple.cpp
 * Describes a 3-tuple that represents a single transistion: < x,y,sigma >
 * when: x is the current state ID
 * 		 y is the next state ID
 * 		 sigma is the input value that caused the transition (sigma must belong to an 8-bit alphabet)
 */

#include "Transition3Tuple.h"
#include <iostream>
#include <sstream>

using namespace std;

/*
 * Initialized the transition 3-tuple
 */
Transition3Tuple::Transition3Tuple(int x,int y, unsigned char sigma)
{
	if (set_X(x) == -1)
		m_x=-1;

	if (set_Y(y) == -1)
		m_y=-1;

	set_Sigma(sigma);

}//end of constructor

int Transition3Tuple::set_X(int x)
{
	int ans=-1;
	if (x>-1)
	{
		m_x=x;
		ans=0;
	}
	else
	{
		cout << "Invalid state ID!";
	}

	return ans;
}//end of set_X


int Transition3Tuple::set_Y(int y)
{
	int ans=-1;
	if (y>-1)
	{
		m_y=y;
		ans=0;
	}
	else
	{
		cout << "Invalid state ID!";
	}

	return ans;
}//end of set_Y()


int Transition3Tuple::set_Sigma(unsigned char sigma)
{

	m_sigma=sigma;
	return 0;
}//end of set_Sigma()

int Transition3Tuple::get_X()
{
	return m_x;
}//end of get_X

int Transition3Tuple::get_Y()
{
	return m_y;
}

unsigned char Transition3Tuple::get_Sigma()
{
	return m_sigma;
}

std::string Transition3Tuple::toString() const
{
	ostringstream stream;
	stream << "<" << "X=" << m_x <<", Y=" << m_y << ", Sigma=" << (int)m_sigma << ">" << endl;
	return stream.str();
}//end of toString()

Transition3Tuple::~Transition3Tuple()
{
	// TODO Auto-generated destructor stub
}

