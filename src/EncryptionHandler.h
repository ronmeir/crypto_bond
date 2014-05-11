/*
 * EncryptionHandler.h
 * Implements all cryptography related methods: Key generations, encryption and decryption.
 */
#include "BilinearMappingHandler.h"
#include "StateMachine.h"
#include "Constants.h"
#include <string>
#include <iostream>

#ifndef ENCRYPTIONHANDLER_H_
#define ENCRYPTIONHANDLER_H_

//  ______                             _   _               _    _                 _ _
// |  ____|                           | | (_)             | |  | |               | | |
// | |__   _ __   ___ _ __ _   _ _ __ | |_ _  ___  _ __   | |__| | __ _ _ __   __| | | ___ _ __
// |  __| | '_ \ / __| '__| | | | '_ \| __| |/ _ \| '_ \  |  __  |/ _` | '_ \ / _` | |/ _ \ '__|
// | |____| | | | (__| |  | |_| | |_) | |_| | (_) | | | | | |  | | (_| | | | | (_| | |  __/ |
// |______|_| |_|\___|_|   \__, | .__/ \__|_|\___/|_| |_| |_|  |_|\__,_|_| |_|\__,_|_|\___|_|
//                          __/ | |
//                         |___/|_|


class EncryptionHandler
{

public:



		// __  __           _              _  __
		//|  \/  |         | |            | |/ /
		//| \  / | __ _ ___| |_ ___ _ __  | ' / ___ _   _
		//| |\/| |/ _` / __| __/ _ \ '__| |  < / _ \ | | |
		//| |  | | (_| \__ \ ||  __/ |    | . \  __/ |_| |
		//|_|  |_|\__,_|___/\__\___|_|    |_|\_\___|\__, |
		//                                           __/ |
		//                                          |___/


		class MSK
		{
		public:
			memberElement g,z,h_start,h_end, e_map_of_g_g_aplha;
			memberElement h_sigma[ALPHABET_SIZE];
			expElement alpha;
			BilinearMappingHandler* mMapper;

			MSK(BilinearMappingHandler* mapper);
			virtual ~MSK();
		};//end of class MSK




		//	  _____                    _     _  __
		//	 / ____|                  | |   | |/ /
		//	| (___   ___  ___ _ __ ___| |_  | ' / ___ _   _
		//	 \___ \ / _ \/ __| '__/ _ \ __| |  < / _ \ | | |
		//	 ____) |  __/ (__| | |  __/ |_  | . \  __/ |_| |
		//	|_____/ \___|\___|_|  \___|\__| |_|\_\___|\__, |
		//											   __/ |
		//											  |___/

		class SK
		{
			public:
				//fields:
				BilinearMappingHandler* mMapper;
				EncryptionHandler::MSK* mMasterKey;
				memberElement* m_D_ElementSet 		 ; // D= {d_0,.....,d_(|Q|-1) from G},D_i is associate to state  q_i
				expElement* m_Rt_ExpSet		   		 ; //Rt={r_0,..r_|T|} r_i is associate to transition t_i from T
				expElement* m_R_end_ExpSet	  		 ; //Rend s.t Rend_x  is associate to q_x ,an acceptance state from Q
				expElement  m_Rstart		  		 ; //a random element from Zp
				memberElement m_K_start1,m_K_start2	 ; //from the article paper
				memberElement** m_K_t				 ; //size() = [3][#transitions]
				memberElement** m_K_for_q_x			 ; //size() = [2][#acceptance-states]
				StateMachine *mM					 ;

				//methods:
				int getIndexInKeyComponentsByTransition(int x, unsigned char sigma);
				SK( BilinearMappingHandler* mapper, StateMachine* M, MSK* msk);
				virtual ~SK();

			};//end of class SK

		//	   _____ _       _            _______        _
		//	  / ____(_)     | |          |__   __|      | |
		//	 | |     _ _ __ | |__   ___ _ __| | _____  _| |_
		//	 | |    | | '_ \| '_ \ / _ \ '__| |/ _ \ \/ / __|
		//	 | |____| | |_) | | | |  __/ |  | |  __/>  <| |_
		//	  \_____|_| .__/|_| |_|\___|_|  |_|\___/_/\_\\__|
		//	          | |
		//	          |_|

		class CT
		{
		private:
			BilinearMappingHandler* mMapper;

		public:
			memberElement** m_Ci; //for each i, it'll hold C_i_1 and all possible C_i_2 (C_i_2 has 256 different values)
			memberElement m_Cm, m_C_start1, m_C_start2, m_C_end1, m_C_end2;
			bool mIsPartialCT;
			std::string mVirus;

			CT(const MSK* msk, int virus_string_length);
			void get_C_i_1 (memberElement& ans, int i);
			void get_C_i_2 (memberElement& ans, int i, unsigned char letter, bool isUsedWithPartialCT);
			virtual ~CT();
		};//end of class CT

	//public methods:
	MSK* setup();
	EncryptionHandler::SK* keyGen();
	void createPartialEncryption (CT& ct,const std::string& w, memberElement& m);
	void completePartialEncryption (CT& ct,const std::string& virus);
	void mapStringToElementFromGT (memberElement& ans, const std::string& str);
	void decrypt(memberElement& decryptedMSG,SK& secretKey, CT& cipherText, StateMachine stateMachine);
	BilinearMappingHandler* getBilinearMappingHandler();
	EncryptionHandler(char* ParamFilePath,  StateMachine* stateMachine);
	virtual ~EncryptionHandler();

private	:
	//member variables
	BilinearMappingHandler* mMapper;
	MSK* mMasterKey;
	StateMachine* mStateMachine;
	SK* mSecretKey;
};//end of class EncryptionHandler


#endif /* ENCRYPTIONHANDLER_H_ */
