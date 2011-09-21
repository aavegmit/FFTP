#include "client.h"
#include "server.h"

void *TCPreadThread(void *args){
    long sockfd ;
    int numbytes;  
    unsigned char header[TCP_HEADER_SIZE];
    uint8_t message_type ;
    uint32_t data_len ;

    sockfd = (long)args ;
    unsigned char *buffer ;
    buffer = (unsigned char *)malloc(1) ;

    // While(1) needs to be changed to some shutdown variable
    while(1){
	memset(header, 0x00, TCP_HEADER_SIZE) ;
	numbytes = (int)read(sockfd, header, TCP_HEADER_SIZE) ;
	if (numbytes != TCP_HEADER_SIZE){
	    fprintf(stderr, "Error in HEADER\n") ; 
	    pthread_exit(0);
	}

	// Get the message type and data length
	memcpy(&message_type, header, 1) ;
	memcpy(&data_len, header + 1, 4) ;

	buffer = (unsigned char *)realloc(buffer, data_len) ;

	numbytes = (int)read(sockfd, buffer, data_len) ;
	if (numbytes != (int)data_len){
	    fprintf(stderr, "Error in buffer\n") ; 
	    pthread_exit(0);
	}

	processReceivedTCPmessage(message_type, buffer, data_len) ;


    }

    free(buffer) ;
    pthread_exit(0);   
}

void *TCPwriteThread(void *){
    printf("In TCP write thread...\n") ;

    pthread_exit(0);
}

void pushMessageInTCPq(uint8_t message_type, unsigned char * buffer, uint32_t data_len){
    // Construct the unsigned char
    // Acquire the tcp write thread lock
    // push the unsigned char in Q
    // Signal the write thread to wake up
    // release the lock
}
