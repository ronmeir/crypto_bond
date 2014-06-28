/*
 * Static_Funcs.h
 *
 *  Created on: Jun 28, 2014
 *      Author: slava
 */

#ifndef STATIC_FUNCS_H_
#define STATIC_FUNCS_H_

#include <string>
#include "SocketWrapper.h"
#include "Constants.h"

using namespace std;

/**
 * Since we want to implement functions in this header file and not only declare them, we chose to use
 * 'inline' to prevent multiple declaration of functions in this header. We're aware that this move will
 * increase the size of the program, but it's relatively negligible.
 */

/*
 * Creates a message according to the format described at Messages.h
 */
inline string createMessage(string src,string dst,string opcode,int content_length, string content)
{
	std::string res(src+SFSC+dst+SFSC+opcode+SFSC);
	//convert the string's length from in to string (similar to itoa()'s result):
	string msg_length_str = static_cast<ostringstream*>( &(ostringstream() << content_length) )->str();
	res.append(msg_length_str+SFSC+content);//length

	return res;
}//end of createMessage()

/*
 * Reads a single message from a socket and parses it by fields.
 * @param sock - the socket
 * @return - a vector containing the message's fields (src,dst,opcode,size,content)
 */
inline vector<string> readAndParseMessageFromSocket(SocketWrapper& sock)
{
	string currentField;
	char currentChar=0;
	vector<string> results;

	//TODO WE HAVE NO TIMEOUT ON THE READ ATTEMPT. CONSIDER ADDING IT.

	//reading the first 4 fields:
	for (int i=0; i<4 ;i++)
	{
		sock.receiveFromSocket(&currentChar,1); //read a single char

		while (currentChar != SFSC) //in the current message field
		{
			currentField.append(""+currentChar);    //append the char to a string
			sock.receiveFromSocket(&currentChar,1); //read the next char
		}//while

		results.push_back(currentField); //push the current field to the result vector
		currentField.clear();            //clear the current field holder
	}//for

	int content_length = atoi(results[3].c_str()); //convert the length string to an int
	currentField.clear();

	//extract the content:
	for (int i=0; i<content_length ;i++)
	{
		sock.receiveFromSocket(&currentChar,1); //read the next char
		currentField.append(""+currentChar);    //append the char to a string
	}//for

	results.push_back(currentField); //add to the vector

	return results;

}//end of readAndParseMessageFromSocket()


#endif /* STATIC_FUNCS_H_ */
