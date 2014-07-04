/*
 * Static_Funcs.h
 *
 *  Created on: Jun 28, 2014
 *      Author: slava
 */

#ifndef STATIC_FUNCS_H_
#define STATIC_FUNCS_H_

#include <string.h>
#include <cstring>
#include "SocketWrapper.h"
#include "Constants.h"

using namespace std;

/**
 * Since we want to implement functions in this header file and not only declare them, we chose to use
 * 'inline' to prevent multiple declaration of functions in this header. We're aware that this move will
 * increase the size of the program, but it's relatively negligible.
 */

/*
 * Converges every byte in the given byte array to be in the range [32,126]
 */
inline void createDisplayableBondPT_String (string& saveHere,const char* arr, int size)
{
	char temp[size];

	memcpy(temp,arr,size);  //copy the input

	int rangeSize = ('~') - (' ') + 1;  //calc the size of the range we're gonna move the bytes to.
	int offset = ' '; //space is the first char in the range.

	for (int i=0; i<size ;i++) //for every given byte
	{
		if (temp[i]<0)       //for some reason, the % operator works only on positive ints.
			temp[i] *= -1;

		temp[i] %= rangeSize;
		temp[i] += offset;
	}

	saveHere.assign(temp,size);

}//end of createDisplayableBondPT_String()

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
	char buff[BUF_SIZE];
	vector<string> results;
	int j;

	//TODO WE HAVE NO TIMEOUT ON THE READ ATTEMPT. CONSIDER ADDING IT.
	//reading the first 4 fields:
	for (int i=0; i<4 ;i++)
	{
		j=0;
		sock.receiveFromSocket(&buff[j],1); //read a single char

		while (buff[j] != SFSC) //in the current message field
		{
			j++;
			sock.receiveFromSocket(&buff[j],1); //read the next char
		}//while

		buff[j]='\0'; //replace the SFSC with a null terminator
		results.push_back(buff); //push the current field to the result vector
	}//for

	int content_length = atoi(results[3].c_str()); //convert the length string to an int

	j=0;
	int i;
	//extract the content:
	for (i=0; i<content_length ;i++)
	{
		sock.receiveFromSocket(&buff[j],1); //read the next char
		j++;
	}//for

	string temp(buff,content_length);  //create a string from the content buffer

	results.push_back(temp); //add to the vector

	return results;

}//end of readAndParseMessageFromSocket()

/*
 * Breaks down a string into tokens, using strtok(). Every Token will be of maximal length MAX_MSG_LENGTH
 * @param saveHere - a 2d char array. the tokes will be copied here, so make sure it has enough allocated memory
 * @param delimsStr - delimiters for strtok()
 * @param str - the string to be tokenized.
 */
//inline void tokenizeString (char** saveHere, const char* delimsStr, const char* str)
//{
//	char* token;
//	int i=0;
//
//	char copy[strlen(str)+1];
//	strcpy(copy,str);   //create a copy of the input string
//
//   /* get the first token */
//   token = strtok(copy, delimsStr);
//
//   /* walk through other tokens */
//   while( token != NULL )
//   {
//	  strncpy(saveHere[i],token,MAX_MSG_LENGTH); //copy the token
//
//	  token = strtok(NULL, delimsStr);
//   }
//}//end of tokenizeString()


#endif /* STATIC_FUNCS_H_ */
