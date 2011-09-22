#include "shared.h"

list<tcpMessage> tcpMessageQ ;
pthread_mutex_t tcpMessageQLock;
pthread_cond_t tcpMessageQCV;


void init(){

    int res = pthread_mutex_init(&tcpMessageQLock, NULL);
    if (res != 0){
	fprintf(stderr, "Lock init failed\n") ;
    }

    res = pthread_cond_init(&tcpMessageQCV, NULL);
    if (res != 0){
	fprintf(stderr, "CV init failed\n") ;
    }
}

void *TCPreadThread(void *args){
    printf("In read thread\n") ;
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

void *TCPwriteThread(void *args){
    printf("In TCP write thread...\n") ;
    long sockfd = (long)args ;
    tcpMessage mes ;

    // Instead of while(1) check to see if shutdown flag is up
    while(1){
	if(tcpMessageQ.size() <= 0){
	    pthread_mutex_lock(&tcpMessageQLock);
	    pthread_cond_wait(&tcpMessageQCV, &tcpMessageQLock);
	    mes = tcpMessageQ.front() ;
	    tcpMessageQ.pop_front() ;
	    pthread_mutex_unlock(&tcpMessageQLock) ;
	    // Check if shutdown flag is up
	}
	else {
	    // Remove one message from the front of the Q
	    mes = tcpMessageQ.front() ;
	    tcpMessageQ.pop_front() ;
	}

	int numBytes = (int)write(sockfd, mes.packet, (int)mes.len) ;
	if(numBytes != (int)mes.len){
	    fprintf(stderr, "TCP write error\n") ;
	}
    }

    pthread_exit(0);
}

void pushMessageInTCPq(uint8_t message_type, unsigned char * buffer, uint32_t data_len){
    // Construct the unsigned char
    tcpMessage mes ;
    mes.packet = (unsigned char *)malloc((int)data_len+5) ;
    memcpy(mes.packet, &message_type, 1) ;
    memcpy(mes.packet+1, &data_len, 4) ;
    memcpy(mes.packet+5, buffer, data_len) ;


    // Acquire the tcp write thread lock
    pthread_mutex_lock(&tcpMessageQLock) ;
    // push the unsigned char in Q
    tcpMessageQ.push_back(mes) ;
    // Signal the write thread to wake up
    pthread_cond_signal(&tcpMessageQCV) ;
    // release the lock
    pthread_mutex_unlock(&tcpMessageQLock) ;
}





