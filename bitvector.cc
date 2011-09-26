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

bool isBitVectorSet(unsigned char *str){
    int noOfBytes ;
    if (objParam.noOfSeq % 8 == 0){
	noOfBytes = objParam.noOfSeq / 8 ;
	for (int i = 0; i < noOfBytes; ++i){
	    if(str[i] != 0xff){
		printf("Byte %d, value - %02x\n", i, str[i]) ;
		return false ;
	    }
	}
    }
    else{
	noOfBytes = objParam.noOfSeq / 8 + 1 ;
	if(noOfBytes > 1){
	    for (int i = 0; i < noOfBytes-1; ++i){
		if((str[i] & 0xff) != 0xff)
		    return false ;
	    }
	}
	for(int i = (noOfBytes - 1)*8 ; i < objParam.noOfSeq - 1; ++i){
	    if(readBit(str, i) != 0x01)
		return false ;
	}
    }
    return true ;

}

void printBitVector(unsigned char *str){
    for(int i = 0 ; i < objParam.noOfSeq ; ++i){
	printf("%x", readBit(str, i)) ;
	if((i+1) % 8 == 0)
	    printf(" ") ;
    }
    printf("\n") ;
}
