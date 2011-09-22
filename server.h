#ifndef _SERVER
#define _SERVER

#include "shared.h"
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <list>


using namespace std ;

//structre for UDP server configurations

struct udpSocketData{

	struct sockaddr_in serv_addr;
	int sockfd;
};

//message Queue for UDP server write thread
extern list<unsigned char* > fileDataList;
extern pthread_mutex_t udpWriteThreadLock;
extern pthread_cond_t udpWriteThreadCV;
// List maintaining all the sequence numbers to be sent

void *TCPserverThread(void *);
void *UDPserverThread(void *);
// This might not be of any use
void *UDPreadThread(void *);
void *UDPwriteThread(void *);
void *prepareBlockThread(void *);

// Handlers
void handleACKlist(unsigned char *, uint32_t) ;
void handleFileName(unsigned char *, uint32_t) ;

// Push the sequence number and the data
void pushBlockInUDPWrite() ;
void pushMessageInTCPq(uint8_t, unsigned char *, uint32_t) ;

void sendAckRequest() ;
#endif
