#ifndef MESSAGES_H_
#define MESSAGES_H_

/*
 * Denote SFSC= Specical Field Seperator Char - This char will separate between the message's fields.
 * Fields:
 * 1. SRC Name
 * 2. DST Name
 * 3. Opcode
 * 4. Content length
 * 5. Content
 *
 * Message Format: SRC|FSFR|DST|FSFR|Opcode|FSFR|Content Length|FSFR|Content
 * When the '|' are just for visual explanation, they won't appear in the message
 */

#define SFSC (char)7  //define the SFSC to be the char with this ascii value

//names:
#define SERVER_NAME "SRVR"
#define CA_NAME "CA"

//message contents:
#define CONTENT_ACK "ACK"
#define CONTENT_VALID "VALID"
#define CONTENT_VALIDATE "VALIDATE"
#define CONTENT_INVALID "INVALID"
#define CONTENT_SM_REQUEST "SM PLZ"
#define CONTENT_NO_CA_APPROVAL "NOT APPROVED BY THE CA YET!"
#define CONTENT_CLIENT_WAS_BUSTED "BUSTED! the following message contained a virus: "
#define CONTENT_SERVER_ECHO_PREFIX "Server echo: "

//message opcodes:
#define OPCODE_CLIENT_TO_SERVER_REQUEST_SM "1"
#define OPCODE_CLIENT_TO_SERVER_SEND_SK "11"
#define OPCODE_CLIENT_TO_SERVER_SEND_BOND "13"
#define OPCODE_CLIENT_TO_SERVER_SEND_MSG "15"

#define OPCODE_CLIENT_TO_CA_SEND_BOND "3"
#define OPCODE_CLIENT_TO_CA_SEND_SK "5"
#define OPCODE_CLIENT_TO_CA_VALIDATE_BOND "7"

#define OPCODE_CA_TO_CLIENT_ACK_BOND "4"
#define OPCODE_CA_TO_CLIENT_ACK_SK "6"
#define OPCODE_CA_TO_CLIENT_REPLY_VALIDATE_BOND "8"
#define OPCODE_CA_TO_CLIENT_SERVICE_UNAVAILABLE "22"

#define OPCODE_CA_TO_SERVER_REQUEST_SM "17"
#define OPCODE_CA_TO_SERVER_APPROVE_CLIENT "9"

#define OPCODE_SERVER_TO_CA_ACK_APPROVE_CLIENT "10"

#define OPCODE_SERVER_TO_CLIENT_OR_CA_REPLY_SM "2"
#define OPCODE_SERVER_TO_CLIENT_ACK_SK "12"
#define OPCODE_SERVER_TO_CLIENT_ACK_BOND "14"
#define OPCODE_SERVER_TO_CLIENT_NOT_APPROVED_BY_CA "18"
#define OPCODE_SERVER_TO_CLIENT_ECHO_MSG "16"
#define OPCODE_SERVER_TO_CLIENT_VIRUS_DETECTED_IN_MSG "19"

#endif
