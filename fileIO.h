/* 
 * fileIO.h
 *
 * This includes all the code related to the Disk I/O
 * For fast disk access, MMAP is also used
 * 
 */

#include "config.h"
#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <list>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// Include the MMAP header file for fast I/O

using namespace std ;

// May be a global variable to hold FILE * and MMAP char *

extern unsigned char *fileMap;
extern struct stat fileStat;
extern list<uint64_t > sequenceNumberList;
// Constructor like function to open file via normal fopen or MMAP open
// Fast Read of a block using MMAP
// Fast write of a block using MMAP

//read from file
void printMMapToFile();
///loads the file to mmap 'fileMap'
void loadFileToMMap();

//unloads the file mapping
void unloadFileMap();

//computes the sequence numbers
void populateSequenceNumberList();

//get blocks of data from file
void getDataFromFile(uint64_t , unsigned char[], uint64_t *);
