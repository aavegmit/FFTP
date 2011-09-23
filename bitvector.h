/*
 * Group - 3-Lost-Packets
 * bitvector.h
 *
 * This file includes the methods related 
 * to the operations on the bitvector 
 * maintained for managing the packets
 *
 */


#include "config.h"
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

using namespace std ;


unsigned char readBit(unsigned char *, uint64_t);
void writeBit(unsigned char *, uint64_t, unsigned char);
void getBitVector(unsigned char *, uint64_t, uint64_t, unsigned char *) ;
