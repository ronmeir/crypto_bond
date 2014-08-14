/*
   _____                           _   ______
  / ____|                         | | |  ____|
 | |  __  ___ _ __   ___ _ __ __ _| | | |__ _   _ _ __   ___ ___
 | | |_ |/ _ \ '_ \ / _ \ '__/ _` | | |  __| | | | '_ \ / __/ __|
 | |__| |  __/ | | |  __/ | | (_| | | | |  | |_| | | | | (__\__ \
  \_____|\___|_| |_|\___|_|  \__,_|_| |_|   \__,_|_| |_|\___|___/


 * Contains a few general usage functions that we use across the project.
 */

#ifndef GENERAL_FUNCS_H_
#define GENERAL_FUNCS_H_

#include <string.h>
#include <string>
#include "SocketWrapper.h"
#include "Constants.h"

using namespace std;

/**
 * Since we want to implement functions in this header file and not only declare them, we chose to use
 * 'inline' to prevent multiple declaration of functions in this header. We're aware that this move will
 * increase the size of the program, but it's relatively negligible.
 */

inline void Quit(int value)
{
	if (g_GUI_threadId)
	{
		pthread_cancel(g_GUI_threadId);
	}
	exit(value);  //shut down.
}//end of Quit()

/*
 * Prints the global params.
 * Should be called after the params were read from the global_param_file
 */
inline void printFileGlobalParams()
{
	cout << endl << endl << "### Global program parameters: ###" << endl;
	cout << "The global parameters that were read from the global_param_file:" << endl << endl;
	cout << PARAM_FROM_FILE_MAX_MSG_LEN << ": " << g_maxMessageLength << endl;
	cout << PARAM_FROM_FILE_SM_SIZE << ": " << g_stateMachineSize << endl;
	cout << PARAM_FROM_FILE_VIRUS_STRING << ": " << g_virus_string << endl << endl;
	cout << "#######################################################" << endl << endl;

}//end of printFileGlobalParams()

/*
 * Checks if two strings are equal, case insensitive.
 * returns true if the strings are equal, false otherwise
 */
inline bool strCmpCaseInsensitive(const char* a,const char* b)
{
	for (;*a && *b; a++,b++)
	{
		if (tolower(*a) != tolower(*b) )
			return false;
	}

	if (*a || *b)			//if the string were of different lengths
	{
		return false;
	}

	return true;
}//end of strCmpCaseInsensitive()

/*
 * Converts a string to an int
 * @param string - the string to be converted
 * @return - the conversion of the string to an int
 */
inline int stringToInt (const char* string)
{
	int numb;
	istringstream ( string ) >> numb;
	return numb;
}//end of stringToInt()

inline bool CheckIfStringContainsDigitsOnly(const char* str)
{
	for (; *str ; str++) //making sure the string contains digits only
	{
		if (*str > 57 || *str < 48)
		{
			return false;
		}
	}

	return true;
}//end of CheckIfStringContainingDigitsOnly()

//the number of bytes that'll be printed in a single row:
#define NUM_OF_BYTES_IN_A_SINGLE_ROW 16

/*
 * Converges every byte in the given byte array to it's hex representation
 * i.e. 30 82 02 5e 02 01 00 02
 *		0f ea 59 6f 1d 32 bf 26
 *		38 ee db ec 91 89 45 01
 *		1d 5d 3a df d8 93 4c 46
 *		1a b9 bb eb 95 de 93 24
 *		d7 6c 2d 43 35 af cf 15
 */
inline void createDisplayableBondPT_String (string& saveHere,const char* arr, int size)
{
	char current_byte[3];
	int currentRowLen = 0;

	current_byte[2]='\0';
	saveHere.clear();

	for (int i=0; i<size ;i++) //for every given byte
	{
		sprintf(current_byte, "%02X", arr[i]); //convert to Hex format

		string temp (current_byte,2); //create a string with current_byte's first 2 bytes

		saveHere += temp + " ";   //append the byte's hex representation

		currentRowLen++;
		currentRowLen %= NUM_OF_BYTES_IN_A_SINGLE_ROW;

		if (currentRowLen==0) //if it's the end of a line
		{
			saveHere += "\n";
		}
	}

}//end of createDisplayableBondPT_String()

///*
// * Converges every byte in the given byte array to be in the range [32,126]
// */
//inline void createDisplayableBondPT_String (string& saveHere,const char* arr, int size)
//{
//	char temp[size];
//
//	memcpy(temp,arr,size);  //copy the input
//
//	int rangeSize = ('~') - (' ') + 1;  //calc the size of the range we're gonna move the bytes to.
//	int offset = ' '; //space is the first char in the range.
//
//	for (int i=0; i<size ;i++) //for every given byte
//	{
//		if (temp[i]<0)       //for some reason, the % operator works only on positive ints.
//			temp[i] *= -1;
//
//		temp[i] %= rangeSize;
//		temp[i] += offset;
//	}
//
//	saveHere.assign(temp,size);
//
//}//end of createDisplayableBondPT_String()

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
 * Breaks the given buffer according to the given delimiters.
 * @param buff - the buffer to be tokenized
 * @param delims - the delimiters
 * @return - a vector with all the tokens
 */
inline vector<string> tokenizeSingleBuffer(const char* buff,const char* delims)
{
	vector<string> results;
	char* token;
	char copy[strlen(buff)+1];

	strcpy(copy,buff);   //make a copy

	/* get the first token */
	token = strtok(copy, delims);

	while (token != NULL)
	{
		string temp(token);  //convert to a string
		results.push_back(temp); //push to the vector

		token = strtok(NULL, delims); //get the next token
	}

	return results;
}//end of tokenizeSingleBuffer()

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
			int retVal = sock.receiveFromSocket(&buff[j],1); //read the next char

			if (retVal < 1)
			{
				return results;
			}
		}//while

		buff[j]='\0'; //replace the SFSC with a null terminator
		results.push_back(buff); //push the current field to the result vector
	}//for

	int content_length = atoi(results[3].c_str()); //convert the length string to an int


	if (content_length > BUF_SIZE) //protect against buffer overflows
	{
		cout  << "THE MESSAGE CONTENT LENGTH IS LARGER THEN THE BUFF SIZE!" << endl;
		//todo remove later:
		throw string("Msg content is way too big!");
	}

	j=0;
	int i;
	//extract the content:
	for (i=0; i < content_length && i <= BUF_SIZE ;i++)
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
