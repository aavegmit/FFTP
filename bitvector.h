/*
 * Group - 3-Lost-Packets
 * bitvector.h
 *
 * This file includes the methods related 
 * to the operations on the bitvector 
 * maintained for managing the packets
 *
 */

#include "shared.h"
#include "bitvector.h"

using namespace std ;

// Get the bitvector so can been used in ACK - char *getBitVector() ;

// Indexed from 0
// Read a bit from the array
unsigned char readBit(unsigned char *str, int location){
	int byte_loc = location / 8  ;
	int bit_loc = location % 8 ;

	unsigned char temp = str[byte_loc] ;

	temp = temp << bit_loc ;
	temp = temp >> 7 ;
	return temp ;
}

// Write a bit at the specified memory location
void writeBit(unsigned char *str, int location, unsigned char value){
	int byte_loc = location / 8  ;
	int bit_loc = location % 8 ;

	unsigned char temp = 0x80 ;
	temp = temp >> bit_loc ;

	if(value == 0x01){
		str[byte_loc] = str[byte_loc] | temp ;
	}
	else{
		str[byte_loc] = str[byte_loc] & ~temp ;
	}
}

unsigned char *getBitVector(){
}
