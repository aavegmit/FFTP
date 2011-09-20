/*
 * Group - 3-Lost-Packets
 * client.h
 *
 * This file includes the methods related
 * to the receiver side
 *
 */

#include <iostream>
#include <cstdio>

using namespace std ;

// As per the specs, the command line is going to be the same
// as scp. something like this: scp aludra.usc.edu:~/file/path/filename destination/file/path/name
// We can store the parsed arguments such has server hostname, server file path, client file path
// in a structure
void parseCommandLine(char **argv) ;

void *TCPconnectionThread(void *);

void *TCPreadThread(void *);

void *TCPwriteThread(void *);

void *UDPconnectionThread(void *);

void *UDPreadThread(void *);

void *UDPwriteThread(void *);
