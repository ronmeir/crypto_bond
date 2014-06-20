/* Constants.h
 * Contains all the constant values used in the project.
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define BUF_SIZE 8192
#define PARAM_FILE_PATH "./param/a.param" //the relative path of the parameter file
#define MAX_MSG_LENGTH 7
#define ALPHABET_SIZE 256
#define SYN_REQUEST_QUEUE_SIZE 10         //Max num of pending syn requests
#define SERVER_AND_CA_TCP_PORT_NUM 80   //define a port number for the welcome sockets on the server and the CA
#define CLIENT_WEB_SERVER_TCP_PORT_NUM 12345 //well known port for HTTP
#define MAX_ELEMENT_LENGTH_IN_BYTES 512

#endif
