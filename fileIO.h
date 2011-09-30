/* 
 * fileIO.h
 *
 * This includes all the code related to the Disk I/O
 * For fast disk access, MMAP is also used
 * 
 */

#ifndef _FILEIO_H
#define _FILEIO_H


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
#include <map>

using namespace std ;

struct fileInfo{
    string fileName;
    struct stat fileStat;
};

extern struct fileInfo fileInfoObj;

typedef struct paramT{
    string serverName;
    string localFilePath;
    string serverFilePath;
    uint64_t fileSize;
    uint64_t noOfSeq;
}param;

extern param objParam;
extern map<long, bool> toBeSend;

extern unsigned char *fileMap;
extern struct stat fileStat;
extern pthread_mutex_t sequenceNumberListLock[NUM_UDP_CONNECTION];
extern pthread_cond_t sequenceNumberListCV[NUM_UDP_CONNECTION];
extern list<uint64_t > sequenceNumberList[NUM_UDP_CONNECTION];
extern unsigned char *mapToFile;
extern bool lastPacketReceived ;
extern unsigned char *bitV ;
extern long packetsRcvd;
void printMMapToFile();
///loads the file to mmap 'fileMap'
void loadFileToMMap();

//unloads the file mapping
void unloadFileMap();

//computes the sequence numbers
void populateSequenceNumberList();

//get blocks of data from file
void getDataFromFile(uint64_t , unsigned char[], uint32_t *);

//function writes to file at client side
void *WriteToFileThread(void *);

//loads MMap to write into file at client side
int loadMMapForFile(unsigned char[]);

//unloads MMap to write into file at client side
void unloadMMapForFile(int);

void pushSequenceNumberInList(uint64_t) ;
#endif
