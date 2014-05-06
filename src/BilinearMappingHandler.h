//  ____  _ _ _                       __  __                   _             _    _                 _ _
// |  _ \(_) (_)                     |  \/  |                 (_)           | |  | |               | | |
// | |_) |_| |_ _ __   ___  __ _ _ __| \  / | __ _ _ __  _ __  _ _ __   __ _| |__| | __ _ _ __   __| | | ___ _ __
// |  _ <| | | | '_ \ / _ \/ _` | '__| |\/| |/ _` | '_ \| '_ \| | '_ \ / _` |  __  |/ _` | '_ \ / _` | |/ _ \ '__|
// | |_) | | | | | | |  __/ (_| | |  | |  | | (_| | |_) | |_) | | | | | (_| | |  | | (_| | | | | (_| | |  __/ |
// |____/|_|_|_|_| |_|\___|\__,_|_|  |_|  |_|\__,_| .__/| .__/|_|_| |_|\__, |_|  |_|\__,_|_| |_|\__,_|_|\___|_|
//                                                | |   | |             __/ |
//                                                |_|   |_|            |___/

#include <pbc.h>
#include "Constants.h"

#ifndef BilinearMappingHandler_H_
#define BilinearMappingHandler_H_

//An element that won't be used as an exponent:
#define memberElement element_t
//An element that'll be used only as an exponent:
#define expElement element_t

class BilinearMappingHandler
{
private:
pairing_t pairing;    //defines all the properties of the group (Zp)

public:

/*
 * Constructor.
 * @param ParamFilePath - the path of the param file (the file that contains the required init data)
 */
BilinearMappingHandler(char* ParamFilePath);
/*
 * Destructor
 */
virtual ~BilinearMappingHandler();
/*
 * Initializes a member element of G1 to no initial value.
 * @param elem - the element to be initialized
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 */
void initEmptyMemberElementFromG1(memberElement& elem);
/*
 * Initializes a member element of G2 to no initial value.
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 * @param elem - the element to be initialized
 */
void initEmptyMemberElementFromG2(memberElement& elem);

/*
 * Initializes an element to a random element from G1.
 * @param elem - the element to be initialized
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 */
void initRandomMemberElementFromG1(memberElement& elem);

/*
 * Initializes an element to a random element from G2.
 * @param elem - the element to be initialized
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 */
void initRandomMemberElementFromG2(memberElement& elem);

/*
 * Initializes an exponent element of Zr to an empty value. Works with all groups
 * @param elem - the element to be initialized
 * EXP ELEMENT MEANS THIS ELEMENT WILL ONLY BE USED AS AN EXPONENT
 */
void initEmptyExpElement(expElement& elem);


/*
 * Initializes an exponent element of Zr to a random value. Works with all groups
 * @param elem - the element to be initialized
 * EXP ELEMENT MEANS THIS ELEMENT WILL ONLY BE USED AS AN EXPONENT
 */
void initRandomExpElement(expElement& elem);


/*
 * Initializes an exponent element of GT to an empty value
 * @param elem - the element to be initialized
 */
void initEmptyMemberElementFromGT(expElement& elem);

/*
 * Initializes an element to a random element from GT.
 * @param elem - the element to be initialized
 * MEMBER ELEMENT MEANS THIS ELEMENT WON'T BE USED AS AN EXPONENT
 */
void initRandomMemberElementFromGT(memberElement& elem);

/*
 * Performs a power operation in Zn: base^exp mod N
 * @param result - the result will be saved here
 */
void power_Zn(memberElement& result, memberElement& base, expElement& exp);

/*
 * Calculates the bilinear mapping e(a,b)
 * @param a - a memberElement of G1
 * @param b - a memberElement of G2
 * @param result - the result will be saved here. This is a member of GT
 */
void bilinearMap(memberElement& result,memberElement& a, memberElement& b);

/**
 * Calculates the inverse element of a
 *  @param result = a^(-1)
 */
void invert(memberElement& result, memberElement& a);


/*
  * Calculates result=a*b
 * @param a - a memberElement of G
 * @param b - a memberElement of G
 * param result - the result will be saved here. this is also member in G
 */
void mul(memberElement& result,memberElement& a,memberElement& b);


/*
 * Compares two elements.
 * @returns 0 if the elements are equal.
 */
int compareElements(element_t& a, element_t& b);

/*
 * Copies an element
 */
void element_cpy(element_t& dst, element_t& src);

/*
 * Returns the length of the given element in bytes.
 * We assume that elements from G1 and G2 can compressed, while elements from GT can't.
 */
int getElementLengthInBytes(element_t elem, bool);

/*
 * Convert an element to a byte array.
 * Elements from G1 or G2 will also be compressed!
 */
void elementToByteArray(unsigned char* saveHere, element_t elem, bool);

/*
 * Convert an unsigned char array to an element
 * Elements from G1 or G2 will also be decompressed!
 */
void byteArrayToElement(element_t saveHere, unsigned char* compressedElem, bool);

};//end of class

#endif
