/*
 * EncryptionHandler.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: slava
 */

#include "EncryptionHandler.h"

using namespace std;

//  ______                             _   _               _    _                 _ _
// |  ____|                           | | (_)             | |  | |               | | |
// | |__   _ __   ___ _ __ _   _ _ __ | |_ _  ___  _ __   | |__| | __ _ _ __   __| | | ___ _ __
// |  __| | '_ \ / __| '__| | | | '_ \| __| |/ _ \| '_ \  |  __  |/ _` | '_ \ / _` | |/ _ \ '__|
// | |____| | | | (__| |  | |_| | |_) | |_| | (_) | | | | | |  | | (_| | | | | (_| | |  __/ |
// |______|_| |_|\___|_|   \__, | .__/ \__|_|\___/|_| |_| |_|  |_|\__,_|_| |_|\__,_|_|\___|_|
//                          __/ | |
//                         |___/|_|

/**
 * this constructor get a paramFilePath , a state-machine and builds everything it needs
 * in order to get a MasterKey, use the function setup()
 * in order to get a SecretKeym use the function keyGen()
 */
EncryptionHandler::EncryptionHandler(char* paramFilePath,  StateMachine* stateMachine)
{
	mMapper 		= new BilinearMappingHandler(paramFilePath);
	mMasterKey 		= new MSK(mMapper);
	mStateMachine	= stateMachine;
	mSecretKey		= new SK(mMapper,mStateMachine,mMasterKey);
}//end of Constructor

EncryptionHandler::~EncryptionHandler()
{
	delete (mMapper);
	delete (mMasterKey);
	delete (mSecretKey);

}//end of Destructor ~EncryptionHandler()

EncryptionHandler::MSK* EncryptionHandler::setup(){ return mMasterKey;} //end of setup()

/**
 * this function returns a SecretKey SK
 * note that SK is a public-nested-class inside the EncryptionHandler!!
 */
EncryptionHandler::SK* EncryptionHandler::keyGen(){return mSecretKey;}//end of KeyGen()

/*
 * Used by the client to create a partial encryption of the bond
 * @param ct - ptr to a CT the result will be saved at
 * @param virus - a random length virus string
 * @param m - an element representing the message.
 */
void EncryptionHandler::createPartialEncryption (CT& ct,const string& w, memberElement& m)
{
	cout << "start creating partial encryption...\n";
	expElement* s;								//ptr array for  S_i
	memberElement tempFrom_GT;
	memberElement tempFromG_1, tempFromG_2;

	mMapper->initEmptyMemberElementFromGT(tempFrom_GT);	//init as a member of GT
	mMapper->initEmptyMemberElementFromG1(tempFromG_1); //init as a member of G
	mMapper->initEmptyMemberElementFromG1(tempFromG_2); //init as a member of G

	int virus_length = MAX_MSG_LENGTH; 	//our implementation defines a constant maximal length
	//Initializing the S_i array:
	s = new expElement[virus_length+1]; 	//create a new exp_element array (virus_length+1) elements

	for (int i=0; i < virus_length+1 ;i++)
		mMapper->initRandomExpElement(s[i]);   //init as a random  exp element
	//done.

	//generating C_m:
	mMapper->power_Zn(tempFromG_1,mMasterKey->g,mMasterKey->alpha); //calc g^alpha
	mMapper->power_Zn(tempFromG_2,mMasterKey->g,s[virus_length]); //calc g^s_l
	mMapper->bilinearMap(tempFrom_GT,tempFromG_1,tempFromG_2);      //calc e(g,g)^(alpha*s_l)
	mMapper->mul(ct.m_Cm,m,tempFrom_GT);                           //calc m * e(g,g)^(alpha*s_l) == Cm

	mMapper->power_Zn(ct.m_C_start1,mMasterKey->g,s[0]);			 	//calc g^s_0 = C_start1
	mMapper->power_Zn(ct.m_C_end1,mMasterKey->g,s[virus_length]); 	//calc g^s_l = C_end1

	mMapper->power_Zn(ct.m_C_start2,mMasterKey->h_start,s[0]);		    //calc (h_start)^s_0 = C_start2
	mMapper->power_Zn(ct.m_C_end2,mMasterKey->h_end,s[virus_length]);//calc (h_end)^s_l = C_end2

	int i;
	//generating C_i:
	for (int col=0; col<virus_length ;col++)
	{
		i=col+1;                     //done so the annotation will be true to the document

		mMapper->power_Zn(ct.m_Ci[0][col],mMasterKey->g,s[i]);   //calc C_i_1 (g^s_i)
		//now we have to fill the remaining 256 cells with all possible C_i_2:
		for (int row=1 ; row <= ALPHABET_SIZE ;row++)
		{
			mMapper->power_Zn(tempFromG_1,mMasterKey->h_sigma[row-1],s[i]);  //calc (h_wi)^s_i
			mMapper->power_Zn(tempFromG_2,mMasterKey->z,s[i-1]);  	//calc z^s_(i-1)
			mMapper->mul(ct.m_Ci[row][col],tempFromG_1,tempFromG_2);  //save the mul result
		}//end of inner for
	}//end of outer for

	delete[] s;
	printf("done with partial encryption\n\n");
}//end of createPartialEncryption()

/**
 * Completes a partial encryption, resulting in a complete encryption.
 * @param partial_ct - A partial encryption that'll be updated to be a full encryption
 * @param virus - the virus string
 */
void EncryptionHandler::completePartialEncryption (CT& partial_ct, const std::string& user_virus)
{
	printf("Completing encryption...\n");
	memberElement** new_m_Ci = new memberElement*[2];  //will be used to hold an array of [2][virus.length()]

	int virus_length  = MAX_MSG_LENGTH;
	//now we have to pad the received virus to the length of MAX_MSG_LENGTH
	//Since our state machine should remain in an acceptance state if such was reach, the decryption alg. should work.
	string virus(user_virus);				  //create a copy of the received virus
	int pad = MAX_MSG_LENGTH - virus.length(); //calc the padding length
	string padStr(pad,0);                     //create a padding string
	virus += padStr;                          //pad the virus string

	//creating a new C_i array that'll replace the current, eliminating all irrelevant h_wi
	new_m_Ci[0] = new memberElement[virus_length];
	new_m_Ci[1] = new memberElement[virus_length];

	//init the elements:
	for (int i=0; i<virus_length ;i++)
	{
		mMapper->initEmptyMemberElementFromG1(new_m_Ci[0][i]);
		mMapper->initEmptyMemberElementFromG1(new_m_Ci[1][i]);
	}

	//copying only the relevant C_i (based on the given virus string):
	for (int col=0; col<virus_length ;col++)
	{
		partial_ct.get_C_i_1(new_m_Ci[0][col],col); //copy C_i_1
		//According to the actual virus string, we need only 1 of the 256 different h_wi:
		partial_ct.get_C_i_2(new_m_Ci[1][col],col,virus.at(col),true);   //copy C_i_2
	}

	//deleting the old array:
	for (int i=0; i<ALPHABET_SIZE+1 ;i++)
		delete[] (partial_ct.m_Ci[i]);

	delete[] partial_ct.m_Ci;

	partial_ct.m_Ci=new_m_Ci;  //set the new array

	partial_ct.mVirus = virus;  //set the virus

	partial_ct.mIsPartialCT=false;       //note that this encryption is no longer partial
	printf(" encryption is done!\n\n");
}//end of completePartialEncryption()

/**
 * Maps a string to an element from GT. Necessary for encryption.
 */
void EncryptionHandler::mapStringToElementFromGT (memberElement& ans, const std::string& str)
{
	//TODO create a mapping table from bond-strings to elements
	mMapper->initRandomMemberElementFromGT(ans);
}//end of mapStringToElementFromGT()

/*
 *
 */
void EncryptionHandler::decrypt(memberElement& decryptedMsgElem, SK& secretKey, CT& cipherText, StateMachine stateMachine)
{
	printf("start decrypting...\n");
	int virusLength=0;
	memberElement B_0, B_i, B_i_minus_1, B_end,temp0, temp1, temp2, temp3;

	mMapper->initEmptyMemberElementFromGT(B_0);  		 //init
	mMapper->initEmptyMemberElementFromGT(B_i);  		 //init
	mMapper->initEmptyMemberElementFromGT(B_i_minus_1);  //init
	mMapper->initEmptyMemberElementFromGT(B_end); 		 //init
	mMapper->initEmptyMemberElementFromG1(temp0); 		 //init
	mMapper->initEmptyMemberElementFromGT(temp1); 		 //init
	mMapper->initEmptyMemberElementFromGT(temp2); 		 //init
	mMapper->initEmptyMemberElementFromGT(temp3); 		 //init

	mMapper->initEmptyMemberElementFromGT(decryptedMsgElem);  //init the result var
	stateMachine.resetMachineToInitialState();           //reset the given state machine

	virusLength = cipherText.mVirus.length();             //get the virus string length

	//calculating B_0:
	mMapper->bilinearMap(temp1, cipherText.m_C_start1, secretKey.m_K_start1);  //calc the 1st mapping
	mMapper->bilinearMap(temp2, cipherText.m_C_start2, secretKey.m_K_start2);  //calc the 2nd mapping
	mMapper->invert(temp2,temp2);											   //temp2 ^ -1
	mMapper->mul(B_0,temp1,temp2);                                             //B_0 = temp1 * temp2

	mMapper->element_cpy(B_i_minus_1,B_0); 									  //cpy B_0 to B_i_minus_1


	stateMachine.resetMachineToInitialState();  //init the state-machine

	//calculating B_i:
	for (int i=1; i <= virusLength ;i++)
	{

		//getting the index of the current transition in the transition table
		//(the transition is defined by the current state and the input):
		int indexAt_K_t = secretKey.getIndexInKeyComponentsByTransition(stateMachine.getCurrentStateID(),
				cipherText.mVirus.at(i-1));

		//move the state machine to the next state:
		stateMachine.moveToNextState(cipherText.mVirus.at(i-1));

		//let the calculations begin:

		if (i==1) //if we're calculating B_1
			mMapper->element_cpy(temp0,cipherText.m_C_start1); //extract C_0,1
		else
			//i>=2
			cipherText.get_C_i_1(temp0,i-2); //extract C_i-1_1

		mMapper->bilinearMap(temp1,temp0,secretKey.m_K_t[0][indexAt_K_t]); //calc the 1st bilinear map

		cipherText.get_C_i_2(temp0,i-1,0,false); //extract C_i_2
		mMapper->bilinearMap(temp2,temp0,secretKey.m_K_t[1][indexAt_K_t]); //calc the 2nd bilinear map
		mMapper->invert(temp2,temp2);										//invert

		cipherText.get_C_i_1(temp0,i-1); //extract C_i_1
		mMapper->bilinearMap(temp3,temp0,secretKey.m_K_t[2][indexAt_K_t]); //calc the 3rd bilinear map

    	mMapper->mul(temp2,temp2,temp3);	//multiplication of the last 2 elements -> temp2
    	mMapper->mul(temp1,temp1,temp2);	//multiplication of the last 3 elements -> temp1
		mMapper->mul(B_i,B_i_minus_1,temp1);   //calc the multiplication of all 4 elements -> B_i


		mMapper->element_cpy(B_i_minus_1,B_i);					//set B_i-1 to be B_i for the next iteration.

	}//for

	//Finished with B_i. At this point: B_i = B_l

	/*getting the index of the current state in the accept-state vector.
	required for a direct access to the m_K_for_q_x	array */
	int indexAt_K_end = stateMachine.getIndexOfAcceptanceStateInTheAcceptanceStatesVector(
			stateMachine.getCurrentStateID());

	//illegal virus check:
	if (indexAt_K_end==-1)
	{
		cout << "The given input didn't result in an acceptance state! decryption failed!";
		return;
	}

	mMapper->bilinearMap(temp1,cipherText.m_C_end1,secretKey.m_K_for_q_x[0][indexAt_K_end]); //map
	mMapper->invert(temp1,temp1);															 //invert

	mMapper->bilinearMap(temp2,cipherText.m_C_end2,secretKey.m_K_for_q_x[1][indexAt_K_end]); //map

	mMapper->mul(temp1,temp1,temp2);    //mul the 2 last elements into temp1
	mMapper->mul(B_end,B_i,temp1);		//b_l * (the 2 last elements)

	/*DONE WITH THE HEAVY CALCULATIONS. THE ONLY THING LEFT NOW IS TO INVERT B_end AND MULTIPLY IT
	 * WITH THE CT TO RECEIVE THE PT
	 */

	mMapper->invert(temp2,B_end);             //invert
	mMapper->mul(decryptedMsgElem,temp2,cipherText.m_Cm); //HALELUJA
	printf("decrypting is done!\n\n");
}//end of decrypt()

/*
 * Returns a BilinearMappingHandler
 */
BilinearMappingHandler* EncryptionHandler::getBilinearMappingHandler() {return mMapper; }


//	  _____                    _     _  __
//	 / ____|                  | |   | |/ /
//	| (___   ___  ___ _ __ ___| |_  | ' / ___ _   _
//	 \___ \ / _ \/ __| '__/ _ \ __| |  < / _ \ | | |
//	 ____) |  __/ (__| | |  __/ |_  | . \  __/ |_| |
//	|_____/ \___|\___|_|  \___|\__| |_|\_\___|\__, |
//											   __/ |
//											  |___/

/**
 * the SK Constructor
 */
EncryptionHandler::SK::SK(BilinearMappingHandler* mapper,StateMachine* M, MSK* msk)
{

	mM=M																 ;//mM holds the state machine
	mMapper=mapper														 ;
	mMasterKey=msk														 ;
	m_D_ElementSet  = new memberElement[M->getTotalNumOfStates()]		 ; //init. size
	m_Rt_ExpSet		= new expElement[M->getTotalNumOfTransitions()]	 	 ; //init. size
	m_R_end_ExpSet	= new expElement[M->getTotalNumOfAcceptenceStates()] ; //init. size


	mMapper->initRandomExpElement(m_Rstart)								 ; //init m_Rstart

	//init. all the random elements in  D_ElementSet
	for(int i =0; i<M->getTotalNumOfStates();i++)
	{
		mMapper->initRandomMemberElementFromG1(m_D_ElementSet[i])	;//init. a random member element @d_i
	}//for

	//init. all the random elements in m_Rt_ExpSet
	for(int i =0; i<M->getTotalNumOfTransitions();i++)
	{
		mMapper->initRandomExpElement(m_Rt_ExpSet[i])				;//init. a random member element @Zp
	}//for

	//init. all the random elements in m_R_end_ExpSet
	for(int i =0; i<M->getTotalNumOfAcceptenceStates();i++){
		mMapper->initRandomExpElement(m_R_end_ExpSet[i])				;//init. a random exp element @Zp
	}//for

	//K_start1= d_0*(h_start)^r_start
	mMapper->initEmptyMemberElementFromG1(m_K_start1)				;//init. k_start1
	mMapper->power_Zn(m_K_start1,mMasterKey->h_start,m_Rstart)		;//k_start1=(h_start)^r_start
	mMapper->mul(m_K_start1,m_K_start1,m_D_ElementSet[0])			;//kk_start1=d_0*(h_start)^r_start

	//K_start2=g^(r_start)
	mMapper->initEmptyMemberElementFromG1(m_K_start2)				;//init. k_start2
	mMapper->power_Zn(m_K_start2,mMasterKey->g,m_Rstart)			;//k_start2==g^(r_start)


	//Generating a 2d array with 3 rows and t columns. The i-th column contains the parameters for Ki


	//allocate the m_Ks 1st dimension
	m_K_t = new memberElement*[3]		;//k.size()= [3][|T|]

	//allocate the m_Ks 2nd dimension
	for(int i=0;i<3;i++)
	{
		m_K_t[i]=new memberElement[M->getTotalNumOfTransitions()];
	}//for

	//init the 2d element array:
	for(int i=0;i<3;i++)
		for(int j=0;j<M->getTotalNumOfTransitions();j++)
		{
			mMapper->initEmptyMemberElementFromG1(m_K_t[i][j]); //init
		}

	//will be used by the constructor:
	const std::vector<Transition3Tuple>* T =M->translateStateMachineToTriples(); //T is a transitions vector
	int x_id,y_id;
	unsigned char sigma;
	memberElement tmp1,tmp2 ;
	mMapper->initEmptyMemberElementFromG1(tmp1)								   ; //init. tmp1
	mMapper->initEmptyMemberElementFromG1(tmp2)								   ; //init. tmp2


	//for each transition tuple: {t=<x,y,sigma>}, we build a 3-tuple:{k_t = <k_t1,k_t2,k_t3>}
	//s.t:   {  k_t1 = < ((d_x)^-1)z^(r_t),	k_t2 = g^(r_t),	k_t3 = d_y*(h_sigma)^(r_t) >  }

	//calculating the 3-tuples set :{k_t}
	for (int i=0;i<M->getTotalNumOfTransitions();i++)
	{

		//Definition of K_t1
		Transition3Tuple t= T->at(i);											//t=T[i]
		x_id =t.get_X();														//gets the ID of x
		mMapper->invert(tmp1,m_D_ElementSet[x_id]);								//tmp1 = (D_x)^(-1)
		mMapper->power_Zn(tmp2,mMasterKey->z,m_Rt_ExpSet[i]);					//tmp2 = z^(r_t)
		mMapper->mul(m_K_t[0][i],tmp1,tmp2);									//multiply
		//// Done With K_t1

		//Definition of K_t2
		mMapper->power_Zn(m_K_t[1][i],mMasterKey->g,m_Rt_ExpSet[i]);			//K_t2= g^(r_t)
		//// Done With K_t2

		//Definition of K_t3
		sigma =t.get_Sigma();													//get sigma
		mMapper->power_Zn(tmp2,mMasterKey->h_sigma[sigma],m_Rt_ExpSet[i]);		//tmp2 = h_sigma^(r_t)
		y_id =t.get_Y();						  								//get the relevant y id
		mMapper->mul(m_K_t[2][i],m_D_ElementSet[y_id],tmp2);					//K_t3 = Dy(h_sigma)^r_t
		//// Done With K_t3

	}//for
		////done with calculating the 3-tuples set :{k_t}

	//for each q_x from F (F is the acceptance-states set) we compute 2 members, K_end_x1 and K_end_x2:
	//K_end_x1= g^(-alpha)*Dx*(h_end)^(r_endx) - from G
	//K_end_x2= g^(r_endx) - from G

	const std::vector<int>* F = M->getAcceptanceStates();						//F is the acceptance states set
	//allocate the m_K_for_q_x 1st dimension
	m_K_for_q_x = new memberElement*[2];										//k.size()= [2][|F|]

	//allocate the m_K_for_q_x 2nd dimension
	m_K_for_q_x[0]=new memberElement[F->size()];
	m_K_for_q_x[1]=new memberElement[F->size()];

	//for each q_x from F (F are the accept states)
	for (int i=0; i<F->size() ;i++){
		//init. m_K_for_q_x[0][i],m_K_for_q_x[1][i]
			mMapper->initEmptyMemberElementFromG1(m_K_for_q_x[0][i]);			//init. K_endx1
			mMapper->initEmptyMemberElementFromG1(m_K_for_q_x[1][i]);			//init. K_endx2

		//Definition of  K_end_x1
			x_id =F->at(i);													   	//get the ID of x
			mMapper->power_Zn(tmp2,mMasterKey->h_end,m_R_end_ExpSet[i]);		//tmp2= (h_end)^r_endx

			mMapper->mul(tmp1,m_D_ElementSet[x_id],tmp2);						//tmp1= D_x*(h_end)^r_endx
			//here we have tmp1= tmp1= D_x*(h_end)^r_endx

			mMapper->power_Zn(tmp2,mMasterKey->g,mMasterKey->alpha);			//tmp2= g^(alpha)
			mMapper->invert(tmp2,tmp2);											//tmp2= g^(-alpha)
			//here we have tmp2= g^(-alpha)

			//to get K_end_x1 we need to do:tmp1*tmp2
			mMapper->mul(m_K_for_q_x[0][i],tmp1,tmp2);							//K_end_x1= g^(-alpha)*Dx*(h_end)^(r_end_x)
		////done with K_end_x1


	//Definition of  K_end_x2
			mMapper->power_Zn(m_K_for_q_x[1][i],mMasterKey->g,m_R_end_ExpSet[i]);//K_endx2= g^(R_end_x)
	////done with K_end_x2
	}//for

}//end of SK Constructor

/*
 * Used by EncryptionHandler::decrypt(). Translates a single transition (x,y,sigma) to an index of the relevant
 * Key_Component triple in the  Key_Component array (memberElement** m_K_t).
 * Basically, it provides the index of the relevant Key_Component triple that's associated with a given transition.
 * @param x - the current state ID
 * @param sigma - the input that caused the transition in the state machine
 * @return the index of the relevant K_ti
 */
int EncryptionHandler::SK::getIndexInKeyComponentsByTransition(int x, unsigned char sigma)
{
	return x*ALPHABET_SIZE + sigma; //the index for a direct access to the desired transition
}//end of getIndexInKeyComponentsByTransition()

EncryptionHandler::SK::~SK()
{
	delete[](m_D_ElementSet)	;
	delete[](m_Rt_ExpSet)		;
	delete[](m_R_end_ExpSet)	;
	delete[](m_K_for_q_x)		;
	delete[](m_K_t)			;


	for(int i=0;i<2;i++){delete(m_K_for_q_x[0]);}
	delete[](m_K_for_q_x);


	for(int i=0;i<3;i++){delete(m_K_t[i]);}//for
	delete[]( m_K_t);

}//end of Destructor

// __  __           _              _  __
//|  \/  |         | |            | |/ /
//| \  / | __ _ ___| |_ ___ _ __  | ' / ___ _   _
//| |\/| |/ _` / __| __/ _ \ '__| |  < / _ \ | | |
//| |  | | (_| \__ \ ||  __/ |    | . \  __/ |_| |
//|_|  |_|\__,_|___/\__\___|_|    |_|\_\___|\__, |
//                                           __/ |
//                                          |___/


/*
 * MSK Constructor.
 * Initializes all the public parameters (PP).
 */
EncryptionHandler::MSK::MSK(BilinearMappingHandler* mapper)
{
	mMapper=mapper;
	mMapper->initRandomMemberElementFromG1(g);   			//init g
	mMapper->initRandomMemberElementFromG1(z);				//init z
	mMapper->initRandomMemberElementFromG1(h_start);		//init h_start
	mMapper->initRandomMemberElementFromG1(h_end);			//init h_end
	mMapper->initRandomExpElement(alpha);					//init alpha

	mMapper->initEmptyMemberElementFromGT(e_map_of_g_g_aplha);

	memberElement g_pow_alpha;
	mMapper->initEmptyMemberElementFromG1(g_pow_alpha); 	//init a new element
	mMapper->power_Zn(g_pow_alpha,g,alpha);					//calc g^alpha
	mMapper->bilinearMap(e_map_of_g_g_aplha,g,g_pow_alpha); //calc e(g,g^alpha), which equals to e(g,g)^alpha

	//initializing h_sigma(s)
	for (int i=0; i<ALPHABET_SIZE ;i++)
	{
		mMapper->initRandomMemberElementFromG1(h_sigma[i]);
	}

}//end of constructor

EncryptionHandler::MSK::~MSK(){}

//	   _____ _       _            _______        _
//	  / ____(_)     | |          |__   __|      | |
//	 | |     _ _ __ | |__   ___ _ __| | _____  _| |_
//	 | |    | | '_ \| '_ \ / _ \ '__| |/ _ \ \/ / __|
//	 | |____| | |_) | | | |  __/ |  | |  __/>  <| |_
//	  \_____|_| .__/|_| |_|\___|_|  |_|\___/_/\_\\__|
//	          | |
//	          |_|

/*
 * @param msk
 */
EncryptionHandler::CT::CT(const MSK* msk, int virusLength)
{
	mMapper=msk->mMapper;
	mIsPartialCT = true;

	m_Ci = new memberElement* [ALPHABET_SIZE+1]; //allocate the 1st dimension
	//allocate the 2nd dimension:
	for (int i=0; i<ALPHABET_SIZE+1 ;i++)
		m_Ci[i] = new memberElement[virusLength];

	//init the 2d element array:
	for (int i=0; i< (ALPHABET_SIZE+1) ;i++)
		for (int j=0; j<virusLength ;j++)
		{
			mMapper->initEmptyMemberElementFromG1(m_Ci[i][j]);  //init as an empty member
		}

	mMapper->initEmptyMemberElementFromGT(m_Cm);
	mMapper->initEmptyMemberElementFromG1(m_C_start1);
	mMapper->initEmptyMemberElementFromG1(m_C_start2);
	mMapper->initEmptyMemberElementFromG1(m_C_end1);
	mMapper->initEmptyMemberElementFromG1(m_C_end2);

}//end of constructor


void EncryptionHandler::CT::get_C_i_1 (memberElement& ans, int i)
{
	mMapper->element_cpy(ans , m_Ci[0][i]);
}

/*
 * Retrieves the relevant C_i_2 for the CT
 * @param ans - reference to where the ans will be saved
 * @param i - the index of the wanted C_i_2
 * @param isUsedWithPartialCT - indicates whether working with a partial CT or not.
 * When using with Partial-CT, the parameter 'letter' indicates the actual C_i_2 that'll be retrieved (since there
 * are 256 of possible C_i_2's when dealing with a Partial-CT)
 * @param letter - The virus letter that's associated with the wanted C_i_2
 */
void EncryptionHandler::CT::get_C_i_2 (memberElement& ans, int i, unsigned char letter, bool isUsedWithPartialCT)
{
	if (isUsedWithPartialCT)
		mMapper->element_cpy(ans , m_Ci[letter+1][i]);
	else
		mMapper->element_cpy(ans , m_Ci[1][i]);
}//end of get_C_i_2()

/*
 * Desctructor
 */
EncryptionHandler::CT::~CT()
{
	int size = (mIsPartialCT ? ALPHABET_SIZE+1 : 2);

	for (int i=0; i<size ;i++)
		delete[] (m_Ci[i]);

	delete[] m_Ci;
}


