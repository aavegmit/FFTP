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
// As per the specs, the command line is going to be the same
// as scp. something like this: scp aludra.usc.edu:~/file/path/filename destination/file/path/name
// We can store the parsed arguments such has server hostname, server file path, client file path
// in a structure
int parseCommandLine(char **argv) ;

void *TCPconnectionThread(void *);

void *TCPreadThread(void *);

void *TCPwriteThread(void *);

void *UDPconnectionThread(void *);

void *UDPreadThread(void *);

void *UDPwriteThread(void *);

#endif

