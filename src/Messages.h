#ifndef MESSAGES_H_
#define MESSAGES_H_

/*
  __  __  _____  _____       _          _                                                  _     _
 |  \/  |/ ____|/ ____|     | |        | |                                                | |   (_)
 | \  / | (___ | |  __ ___  | |__   ___| |___      _____  ___ _ __    _ __ ___   __ _  ___| |__  _ _ __   ___  ___
 | |\/| |\___ \| | |_ / __| | '_ \ / _ \ __\ \ /\ / / _ \/ _ \ '_ \  | '_ ` _ \ / _` |/ __| '_ \| | '_ \ / _ \/ __|
 | |  | |____) | |__| \__ \ | |_) |  __/ |_ \ V  V /  __/  __/ | | | | | | | | | (_| | (__| | | | | | | |  __/\__ \
 |_|  |_|_____/ \_____|___/ |_.__/ \___|\__| \_/\_/ \___|\___|_| |_| |_| |_| |_|\__,_|\___|_| |_|_|_| |_|\___||___/

 */

/*
 * Denote SFSC= Specical Field Seperator Char - This char will separate between the message's fields.
 * Fields:
 * 1. SRC Name
 * 2. DST Name
 * 3. Opcode
 * 4. Content length
 * 5. Content
 *
 * Message Format: SRC|SFSC|DST|SFSC|Opcode|SFSC|Content Length|SFSC|Content
 * When the '|' are just for visual explanation, they won't appear in the message
 */

#define SFSC (char)7  //define the SFSC to be the char with this ascii value

//names:
#define SERVER_NAME "SRVR"
#define CA_NAME "CA"
#define UI_SERVER "UI Server"
#define UI_CLIENT "UI Client"

//message contents:
#define CONTENT_ACK "ACK"
#define CONTENT_VALID "VALID"
#define CONTENT_VALIDATE "VALIDATE"
#define CONTENT_INVALID "INVALID"
#define CONTENT_NO_SK_OR_BOND "NO SK OR BOND"
#define CONTENT_NO_SK_AND_BOND "NO SK AND BOND"
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


/*
 * _____ _ _            _    _    _ _____    _____                                            _   _    _ _____        _ _            _
  / ____| (_)          | |  | |  | |_   _|  / ____|                                          | | | |  | |_   _|      | (_)          | |
 | |    | |_  ___ _ __ | |_ | |  | | | |   | (___   ___ _ ____   _____ _ __    __ _ _ __   __| | | |  | | | |     ___| |_  ___ _ __ | |_
 | |    | | |/ _ \ '_ \| __|| |  | | | |    \___ \ / _ \ '__\ \ / / _ \ '__|  / _` | '_ \ / _` | | |  | | | |    / __| | |/ _ \ '_ \| __|
 | |____| | |  __/ | | | |_ | |__| |_| |_   ____) |  __/ |   \ V /  __/ |    | (_| | | | | (_| | | |__| |_| |_  | (__| | |  __/ | | | |_
  \_____|_|_|\___|_| |_|\__| \____/|_____| |_____/ \___|_|    \_/ \___|_|     \__,_|_| |_|\__,_|  \____/|_____|  \___|_|_|\___|_| |_|\__|
 *
 */


#define CONTENT_CA_APPROVED "The SK and Bond were approved by the CA and forwarded to the Server!"
#define CONTENT_CA_DISAPPROVED "The SK and Bond were rejected by the CA!"
#define CONTENT_CA_DIDNT_GET_SK_OR_BOND "The CA is missing the SK or the Bond!"
#define CONTENT_CA_DIDNT_GET_SK_AND_BOND "The CA hasn't received anything from us so far!"
#define CONTENT_CA_SENT_UNKNOWN_REPLY "An unknown reply was received from the CA!"
#define CONTENT_CA_SERVICE_UNAVAILABLE "The CA wasn't able to contact the Server thus far. \nService unavailable!"
#define CONTENT_GET_SM_OK "Successfully received the state machine from the Server!"
#define CONTENT_GET_SM_ERROR "Failed to receive a state machine from the Server!"
#define CONTENT_CANT_CREATE_SK_AND_BOND "Failed to create SK and Bond because no state machine is set!"

#define CONTENT_SERVER_ACK "The SK and Bond were received by the Server!"
#define CONTENT_SERVER_SENT_UNKNOWN_REPLY "An unknown reply was received from the Server!"

#define OPCODE_UI_CLIENT_TO_SERVER_REQUEST_SM_FROM_SERVER "0"
#define OPCODE_UI_CLIENT_TO_SERVER_CREATE_SK_AND_BOND "1"
#define OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_CA "2"
#define OPCODE_UI_CLIENT_TO_SERVER_SEND_SK_AND_BOND_TO_SERVER "3"
#define OPCODE_UI_CLIENT_TO_SERVER_SEND_MSG_TO_SERVER "4"

#define OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_REQUEST_SM_OK "00"
#define OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_REQUEST_SM_ERROR "01"
#define OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_CREATE_SK_AND_BOND_OK "10"
#define OPCODE_UI_SERVER_TO_CLIENT_REPLY_ON_CREATE_SK_AND_BOND_ERROR "11"
#define OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_APPROVED_BY_CA "20"
#define OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_APPROVED_BY_CA "21"
#define OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_RECEIVED_BY_SERVER "30"
#define OPCODE_UI_SERVER_TO_CLIENT_SK_AND_BOND_NOT_RECEIVED_BY_SERVER "31"
#define OPCODE_UI_SERVER_TO_CLIENT_SERVER_REPLY "4"

#endif
