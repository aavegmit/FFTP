#include "bitvector.h"

uint64_t seq_num_count = 10000 ;

// Indexed from 0
// Read a bit from the array
unsigned char readBit(unsigned char *str, uint64_t location){
	int byte_loc = location / 8  ;
	int bit_loc = location % 8 ;

	unsigned char temp = str[byte_loc] ;

	temp = temp << bit_loc ;
	temp = temp >> 7 ;
	return temp ;
}

// Write a bit at the specified memory location
void writeBit(unsigned char *str, uint64_t location, unsigned char value){
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

void getBitVector(unsigned char *str, uint64_t seq_num, uint64_t last_seq_num, unsigned char *buffer){
	memcpy(buffer, str + seq_num/8, (last_seq_num - seq_num) / 8 + 1) ;
}
