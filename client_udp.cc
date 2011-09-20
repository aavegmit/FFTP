#include "client.h"

// This thread is invoked from the client main function
// after command line parsing. It includes setting up a 
// UDP connection with the UDP server
void *UDPconnectionThread(void *){

}

// This thread is invoked after a UDP connection has 
// been established. It is responsible to receive all
// the file content, write to file, and update the
// bitvector
void *UDPreadThread(void *){

}

// This thread might not be of any use, so we wont
// implement it now
void *UDPwriteThread(void *){

}
