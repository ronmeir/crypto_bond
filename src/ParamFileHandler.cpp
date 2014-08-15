/*
 * ParamFileHandler.cpp
 *
 *  Created on: Aug 14, 2014
 *      Author: slava
 */

#include "ParamFileHandler.h"

using namespace std;

ParamFileHandler::ParamFileHandler(const char* filePath)
{
	m_filePath.assign(filePath);
}

/*
 * Initialized the given state machine according to the states described in the param file
 */
bool ParamFileHandler::loadStateMachineFromFile(StateMachine& SM)
{
	FILE* fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	vector<string> singleLine;
	int current_StateNum = -1;
	int current_NumOfTransitions = 0;
	bool current_IsAcceptance = false;

	int transitions[ALPHABET_SIZE][2];
	cout<<"loading SM from file!!!!!"<<endl;

	fp = fopen(m_filePath.c_str(), "r"); //open the global_param_file
	if (fp == NULL)
	{
		return false;
	}

	//search for the beginning of the SM in the file:
	while ((read = getline(&line, &len, fp)) != -1 \
			&& strcmp(line,SM_START_LABEL))
	{}

	if (!read)  //the end of file was reached before an SM_START_LABEL was found
	{
		return false;
	}

	//The next line should be the 1st state.

	//continue reading until the end of the file or the end of the SM
	while ((read = getline(&line, &len, fp)) != -1 && strcmp(line,SM_END_LABEL))
	{
		singleLine = tokenizeSingleBuffer(line," "); //tokenize the current line
		/*if there are more then 3 or less then 2
	 	 arguments in a single line: */
		if (singleLine.size()!=2 || singleLine.size()!=3)
		{
			return false;
		}


		if (!singleLine.at(0).compare("State")) //if this line is the beginning of a new state
		{
			if (current_StateNum != -1) //if this isn't the 1st state in the file
			{
				//add the previous state to the SM
				SM.addState(current_StateNum,transitions,current_NumOfTransitions,current_IsAcceptance);
				current_NumOfTransitions = 0; //init
			}

			//check that the 2nd arg is a valid number:

			if (!CheckIfStringContainsDigitsOnly(singleLine.at(1).c_str()))//making sure the string contains digits only
			{
				return false;
			}

			current_StateNum = stringToInt(singleLine.at(1).c_str());

			//if the state number is illegal:
			if (current_StateNum >= g_stateMachineSize || current_StateNum < 0)
			{
				cout << "File contains a state number that is not in the range [0,num_of_states_in_SM)!" << endl;
				return false;
			}

			//check if the state is an acceptance state or not:
			int len = singleLine.at(2).size();
			singleLine.at(2).erase(len-1,1); //erase the last char, which is '\n'

			if (strCmpCaseInsensitive(singleLine.at(2).c_str(),"false"))
			{
				current_IsAcceptance = false;
			}
			else
			{
				if (strCmpCaseInsensitive(singleLine.at(2).c_str(),"true"))
				{
					current_IsAcceptance = true;
				}
				else //illegal value at the 3rd param
				{
					return false;
				}
			}
		}//if State
		else //should be a transition
		{
			//if the user has entered too many transitions:
			if (current_NumOfTransitions >= ALPHABET_SIZE)
			{
				return false;
			}

			//get the 1st argument (the input):
			transitions[current_NumOfTransitions][0] = singleLine.at(0).at(0); //get the input char

			//get the 2nd argument (the next state):

			int len = singleLine.at(1).size();
			singleLine.at(1).erase(len-1,1); //erase the last char, which is '\n'
			if (!CheckIfStringContainsDigitsOnly(singleLine.at(1).c_str())) //making sure the string contains digits only
			{
				return false;
			}

			transitions[current_NumOfTransitions][1] = stringToInt(singleLine.at(1).c_str()); //save the next state

			//if the state number is illegal:
			if (transitions[current_NumOfTransitions][1] >= g_stateMachineSize || \
					transitions[current_NumOfTransitions][1] < 0)
			{
				cout << "File contains a state number that is not in the range [0,num_of_states_in_SM)!" << endl;
				return false;
			}

			current_NumOfTransitions++; //advance
		}//else


	}//while

	if (!read)  //the end of file was reached before an SM_END_LABEL was found
	{
		return false;
	}

	//the file is valid. Still have to save the last state:
	SM.addState(current_StateNum,transitions,current_NumOfTransitions,current_IsAcceptance);

	if (fp)
	{
		fclose(fp);
	}
	return true;

}//end of loadStateMachineFromFile()

/*
 * Loads the global variables from the file
 */
bool ParamFileHandler::loadGlobalParamsFromFile()
{
	FILE* fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	vector<string> singleLine;
	bool wereAllParamsRead = false;

	fp = fopen(m_filePath.c_str(), "r"); //open the global_param_file
	if (fp == NULL)
	{
		return false;
	}

	while ((read = getline(&line, &len, fp)) != -1) //read a single line
	{
		if (line[0] == '#' || line[0] == ' ' || line[0] == '\r' || line[0] == '\n' \
				|| line[0] == '\t') //this line should be ignored
		{
			continue;
		}

		singleLine = tokenizeSingleBuffer(line," "); //tokenize the current line

		//checking to see what parameter we've just read:
		if (!singleLine.at(0).compare(PARAM_FROM_FILE_MAX_MSG_LEN))
		{
			int len = singleLine.at(2).size();
			singleLine.at(2).erase(len-1,1); //erase the last char, which is '\n'
			if (!CheckIfStringContainsDigitsOnly(singleLine.at(2).c_str())) //making sure the string contains digits only
			{
				break;
			}

			g_maxMessageLength = stringToInt(singleLine.at(2).c_str());
		}//if PARAM_FROM_FILE_MAX_MSG_LEN

		if (!singleLine.at(0).compare(PARAM_FROM_FILE_SM_SIZE))
		{
			int len = singleLine.at(2).size();
			singleLine.at(2).erase(len-1,1); //erase the last char, which is '\n'
			if (!CheckIfStringContainsDigitsOnly(singleLine.at(2).c_str())) //making sure the string contains digits only
			{
				break;
			}

			g_stateMachineSize = stringToInt(singleLine.at(2).c_str());
		}//if PARAM_FROM_FILE_SM_SIZE

		if (!singleLine.at(0).compare(PARAM_FROM_FILE_VIRUS_STRING))
		{
			int len = singleLine.at(2).size();
			g_virus_string.assign(singleLine.at(2).erase(len-1,1));
		}//if PARAM_FROM_FILE_VIRUS_STRING

		//check if we've finished:

		//if we've read all the expected global params
		if (g_maxMessageLength > 0 && g_stateMachineSize > 0 && g_virus_string.size() != 0)
		{
			wereAllParamsRead = true;
			break;
		}
	}//while

	if (line)
		free(line);

	if (fp)
	{
		fclose(fp);  //close the file
	}

	return wereAllParamsRead;
}//end loadGlobalParamsFromFile()


ParamFileHandler::~ParamFileHandler()
{
}


