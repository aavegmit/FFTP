#include "config.h"
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <list>

using namespace std ;

void *TCPreadThread(void *);
void *TCPwriteThread(void *);

typedef struct tcpMessageT {
	unsigned char *packet;
	uint32_t len;
} tcpMessage ;

void processReceivedTCPmessage(uint8_t, unsigned char *, uint32_t) ;
void init() ;

extern list<tcpMessage > tcpMessageQ ;
extern pthread_mutex_t tcpMessageQLock;
extern pthread_cond_t tcpMessageQCV;
