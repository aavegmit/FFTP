#ifndef _COMPRESS_H
#define _COMPRESS_H

/* 
 * Group - 3-Lost-Packets
 * compress.h
 *
 * This file has the methods related to the compression
 * algorithm used in this protocol
 *
 */

// TODO - To think of a compression algo
//      - Whether to compress a particular packet or the whole file?

#include <iostream>
#include <cstdio>
#include "config.h"
#include <stdint.h>

using namespace std ;

// Get Compressed data - char *getCompressedData(char *, int len)
//		       - Implement the compression algo here
void initializeCompression();
unsigned char* compressingBuffer(unsigned char[], uint32_t*);
unsigned char* decompressingBuffer(unsigned char[], uint32_t*);

#endif
