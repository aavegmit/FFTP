#include <iostream>
#include <cstdio>

using namespace std ;

// List maintaining all the sequence numbers to be sent

void *TCPserverThread(void *);

void *TCPreadThread(void *);

void *TCPwriteThread(void *);

void *UDPserverThread(void *);

// This might not be of any use
void *UDPreadThread(void *);

void *UDPwriteThread(void *);

void *prepareBlockThread(void *);

// Sets the bit vector
// If a packet is lost, then add it in the  FRONT of the list of
// sequence numbers
// Write to cache
// Clear out the cache for which packets has been received
void processACK(unsigned char *) ;

// Creates a list of all the sequence numbers
void processMetaData(unsigned char *) ;

// Push the sequence number and the data
void pushBlockInUDPWrite() ;
