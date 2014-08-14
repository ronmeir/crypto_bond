//  ____  _ _ _                       __  __                   _             _    _                 _ _
// |  _ \(_) (_)                     |  \/  |                 (_)           | |  | |               | | |
// | |_) |_| |_ _ __   ___  __ _ _ __| \  / | __ _ _ __  _ __  _ _ __   __ _| |__| | __ _ _ __   __| | | ___ _ __
// |  _ <| | | | '_ \ / _ \/ _` | '__| |\/| |/ _` | '_ \| '_ \| | '_ \ / _` |  __  |/ _` | '_ \ / _` | |/ _ \ '__|
// | |_) | | | | | | |  __/ (_| | |  | |  | | (_| | |_) | |_) | | | | | (_| | |  | | (_| | | | | (_| | |  __/ |
// |____/|_|_|_|_| |_|\___|\__,_|_|  |_|  |_|\__,_| .__/| .__/|_|_| |_|\__, |_|  |_|\__,_|_| |_|\__,_|_|\___|_|
//                                                | |   | |             __/ |
//                                                |_|   |_|            |___/

/*
 * This class simplifies relevant methods from the PBC library.
 */

#include "BilinearMappingHandler.h"

/*
 * Constructor
 */
BilinearMappingHandler::BilinearMappingHandler(char* ParamFilePath)
{
	char param[1024]; //the parameter file will be read here
	FILE* paramFile;

	//open the group-param file:
	paramFile = fopen ( ParamFilePath , "rb" );
	if (paramFile==NULL)
	{
		Quit(1);
	}

	//read the file:
	size_t count = fread(param, 1, 1024, paramFile);

	fclose(paramFile);  //close the file stream

	//use the file to initialize the pairing
	pairing_init_set_buf(pairing, param, count); //init the pairing (which also defines the groups)
}//end of constructor

/*
 * Destructor
 */
BilinearMappingHandler::~BilinearMappingHandler(){}

/*
 * Initializes a member element of G1 to no initial value.
 * @param elem - the element to be initialized
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 */
void BilinearMappingHandler::initEmptyMemberElementFromG1(memberElement& elem)
{
	element_init_G1(elem, pairing);
}//end of initEmptyMemberElementFromG1()


/**
 * Calculates the inverse element of a
 *  @param result = a^(-1)
 */
void BilinearMappingHandler::invert(memberElement& result, memberElement& a){
	 element_invert(result,a);
}//end of invert()


/*
 * Initializes a member element of G2 to no initial value.
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 * @param elem - the element to be initialized
 */
void BilinearMappingHandler::initEmptyMemberElementFromG2(memberElement& elem)
{
	element_init_G2(elem, pairing);
}//end of initEmptyMemberElementFromG2()

/*
 * Initializes an exponent element of GT to an empty value
 * @param elem - the element to be initialized
 */
void BilinearMappingHandler::initEmptyMemberElementFromGT(memberElement& elem)
{
	element_init_GT(elem, pairing);
}//end of initEmptyMemberElementFromGT()

void BilinearMappingHandler::initRandomMemberElementFromGT(memberElement& elem)
{
	element_init_GT(elem, pairing);
	element_random(elem);
}//end of initRandomMemberElementFromGT()

/*
 * Initializes an exponent element of Zr to an empty value. Works with all groups
 * @param elem - the element to be initialized
 * EXP ELEMENT MEANS THIS ELEMENT WILL ONLY BE USED AS AN EXPONENT
 */
void BilinearMappingHandler::initEmptyExpElement(memberElement& elem)
{
	element_init_Zr(elem, pairing);
}//end of initEmptyExpElementFromG1()

/*
 * Initializes an element to a random element from G1.
 * @param elem - the element to be initialized
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 */
void BilinearMappingHandler::initRandomMemberElementFromG1(memberElement& elem)
{
	initEmptyMemberElementFromG1(elem);
	element_random(elem);
}//end of initRandomMemberElementFromG1()


/*
 * Initializes an element to a random element from G2.
 * @param elem - the element to be initialized
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 */
void BilinearMappingHandler::initRandomMemberElementFromG2(memberElement& elem)
{
	initEmptyMemberElementFromG2(elem);
	element_random(elem);
}//end of initRandomMemberElementFromG2()

/*
 * Initializes an exponent element of Zr to a random value. Works with all groups
 * @param elem - the element to be initialized
 * EXP ELEMENT MEANS THIS ELEMENT WILL ONLY BE USED AS AN EXPONENT
 */
void BilinearMappingHandler::initRandomExpElement(memberElement& elem)
{
	initEmptyExpElement(elem);
	element_random(elem);
}//end of initRandomExpElement()

/*
 * Performs a power operation in Zn: base^exp mod N
 * @param result - the result will be saved here
 */
void BilinearMappingHandler::power_Zn(memberElement& result, memberElement& base, expElement& exp)
{
	element_pow_zn(result, base, exp); //result = base^exp
}//end of power_Zn()


/*
 * Calculates result=a*b
 * @param a - a memberElement of G
 * @param b - a memberElement of G
 * param result - the result will be saved here. this is also member in G
 */
void BilinearMappingHandler::mul(memberElement& result,memberElement& a,memberElement& b){
	 element_mul(result, a,b);
}

/*
 * Calculates the bilinear mapping e(a,b)
 * @param a - a memberElement of G1
 * @param b - a memberElement of G2
 * @param result - the result will be saved here. This is a member of GT
 */
void BilinearMappingHandler::bilinearMap(memberElement& result,memberElement& a, memberElement& b)
{
	pairing_apply(result, a, b, pairing); //perform bilinear mapping
}//end of bilinearMap()

/*
 * Compares two elements.
 * @returns 0 if the elements are equal.
 */
int BilinearMappingHandler::compareElements(element_t& a, element_t& b)
{
	return element_cmp(a,b);
}//end of compareElements()

/*
 * Copies src into dst
 */
void BilinearMappingHandler::element_cpy(element_t& dst, element_t& src)
{
	element_set(dst,src);
}//end of element_cpy()

/*
 * Returns the length of the given element in bytes.
 * We assume that elements from G1 and G2 can be compressed, while elements from GT can't.
 */
int BilinearMappingHandler::getElementLengthInBytes(element_t elem, bool isElementOfGT)
{
	if (isElementOfGT)
		return element_length_in_bytes(elem);   //members of GT can't be compressed
	else
		return element_length_in_bytes_compressed(elem);  //members of G1/G2 can be compressed
}//end of getElementLengthInByte()

/*
 * Convert an element to a byte array.
 * Elements from G1 or G2 will also be compressed!
 */
void BilinearMappingHandler::elementToByteArray(unsigned char* saveHere, element_t elem, bool isElementOfGT)
{
	if (isElementOfGT)
		element_to_bytes(saveHere,elem);
	else
		element_to_bytes_compressed(saveHere, elem);
}//end of elementToByteArray()

/*
 * Convert an unsigned char array to an element
 * Elements from G1 or G2 will also be decompressed!
 */
void BilinearMappingHandler::byteArrayToElement(element_t saveHere, unsigned char* compressedElem, bool isElementOfGT)
{
	if (isElementOfGT)
		element_from_bytes(saveHere,compressedElem);
	else
		element_from_bytes_compressed(saveHere, compressedElem);
}//end of byteArrayToElement()




