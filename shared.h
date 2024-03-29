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
#include <sys/wait.h>
#include "bitvector.h"
#include "fileIO.h"

using namespace std ;

void *TCPreadThread(void *);
void *TCPwriteThread(void *);

typedef struct tcpMessageT {
	unsigned char *packet;
	uint32_t len;
} tcpMessage ;

typedef struct udpMessageT {
	unsigned char buffer[MAXDATASIZE];
	uint64_t sequenceNum;
	uint32_t data_len;
} udpMessage ;

void processReceivedTCPmessage(uint8_t, unsigned char *, uint32_t) ;
void init() ;
void initUDP() ;

void pushMessageInUDPq(uint64_t, uint32_t, unsigned char* , int);
void pushMessageInTCPq(uint8_t, unsigned char *, uint32_t) ;
//creates a udp packet for caching purposes
udpMessage getUDPpacketFromData(uint64_t, uint32_t, unsigned char*);
void displayStats();
//For shutdown purposes
void shutDown();

extern long noOfPacketsSent[NUM_UDP_CONNECTION];
extern long uptoPacketSent[NUM_UDP_CONNECTION] ;
extern long noOfAckSent[NUM_UDP_CONNECTION] ;
extern long noOfAckRecd[NUM_UDP_CONNECTION] ;
extern long long noOfLossPackets ;
extern long udpSendWaitCount[NUM_UDP_CONNECTION];

extern list<tcpMessage > tcpMessageQ ;
extern pthread_mutex_t tcpMessageQLock;
extern pthread_cond_t tcpMessageQCV;

extern unsigned char *bitV ;
//lock and CV for UDP write thread
//message Queue for UDP server write thread
extern list<udpMessage > udpMessageQ[NUM_UDP_CONNECTION];
extern pthread_mutex_t udpMessageQLock[NUM_UDP_CONNECTION];
extern pthread_cond_t udpMessageQCV[NUM_UDP_CONNECTION];

extern list<udpMessage > udpMessageClientQ;
extern pthread_mutex_t udpMessageClientQLock;
extern pthread_cond_t udpMessageClientQCV;
extern pthread_mutex_t packetSentLock ;

extern int udpPortList[20];
extern unsigned char clientName[255];


extern bool shutDownFlag ;
extern list<pthread_t > listOfThreads;
extern list<int > listOfSockfd;
extern uint64_t dropPacketCount ;
#endif
