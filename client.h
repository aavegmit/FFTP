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

#include "shared.h"
using namespace std ;

//structre for UDP server configurations
struct udpSocketData{

        struct sockaddr_in serv_addr;
        int sockfd;
};


typedef struct paramT{
    string serverName;
    string localFilePath;
    string serverFilePath;
    uint64_t fileSize;
    uint64_t noOfSeq;
}param;

extern param objParam;

//Threads 
void *TCPconnectionThread(void *);
void *UDPconnectionThread(void *);
void *UDPreadThread(void *);
void *UDPwriteThread(void *);

// Methods
int parseCommandLine(char **argv) ;
//void pushMessageInTCPq(uint8_t, unsigned char *, uint32_t) ;

// Message handlers
void handleFileInfo(unsigned char *, uint32_t data_len) ;
void handleFileNotFound(unsigned char *, uint32_t data_len) ;
void handleAckRequest(unsigned char *, uint32_t data_len) ;

#endif
