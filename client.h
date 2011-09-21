/*
 * Group - 3-Lost-Packets
 * client.h
 *
 * This file includes the methods related
 * to the receiver side
 *
 */
#ifndef _CLIENT
#define _CLIENT

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
using namespace std ;

typedef struct paramT{
    string serverName;
    string localFilePath;
    string serverFilePath;
}param;

extern param objParam;

//Threads 
void *TCPconnectionThread(void *);
void *TCPreadThread(void *);
void *TCPwriteThread(void *);
void *UDPconnectionThread(void *);
void *UDPreadThread(void *);
void *UDPwriteThread(void *);

// Methods
int parseCommandLine(char **argv) ;
void processReceivedTCPmessage(uint8_t, unsigned char *, uint32_t) ;
void pushMessageInTCPq(uint8_t, unsigned char *, uint32_t) ;

// Message handlers
void handleFileInfo(unsigned char *, uint32_t data_len) ;
void handleFileNotFound(unsigned char *, uint32_t data_len) ;
void handleAckRequest(unsigned char *, uint32_t data_len) ;

#endif
