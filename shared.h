#ifndef _SHARED_H
#define _SHARED_H

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
#include <signal.h>
#include <pthread.h>
#include <list>

using namespace std ;

void *TCPreadThread(void *);
void *TCPwriteThread(void *);

typedef struct tcpMessageT {
	unsigned char *packet;
	uint32_t len;
} tcpMessage ;

typedef struct udpMessageT {
	unsigned char *buffer;
	uint64_t sequenceNum;
	uint32_t data_len;
} udpMessage ;

void processReceivedTCPmessage(uint8_t, unsigned char *, uint32_t) ;
void init() ;
void initUDP() ;

void pushMessageInUDPq(uint64_t, uint32_t, unsigned char* );
void pushMessageInTCPq(uint8_t, unsigned char *, uint32_t) ;
//creates a udp packet for caching purposes
udpMessage getUDPpacketFromData(uint64_t, uint32_t, unsigned char*);

extern list<tcpMessage > tcpMessageQ ;
extern pthread_mutex_t tcpMessageQLock;
extern pthread_cond_t tcpMessageQCV;

//lock and CV for UDP write thread
//message Queue for UDP server write thread
extern list<udpMessage > udpMessageQ;
extern pthread_mutex_t udpMessageQLock;
extern pthread_cond_t udpMessageQCV;

extern int udpPortList[5];
extern unsigned char clientName[255];

#endif
