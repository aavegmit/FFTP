#include "client.h"
// This thread is invoked from the client main function
// after command line parsing. It includes setting up a 
// UDP connection with the UDP server
void *UDPconnectionThread(void *){

    int sockfd[NUM_UDP_CONNECTION];

    struct udpSocketData *udpSocketDataObj;
    struct sockaddr_in serv_addr[NUM_UDP_CONNECTION];
    pthread_t udpReadThread[NUM_UDP_CONNECTION];
    pthread_t udpWriteThread[NUM_UDP_CONNECTION];

    //Creating a DATAGRAM socket
    for(int i = 0;i<NUM_UDP_CONNECTION;i++){
	sockfd[i] = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd[i] < 0){ 
	    printf("printf opening UDP sockets!!!\n");
	    exit(0);
	} 

	bzero((char *) &serv_addr[i], sizeof(serv_addr[i]));

	serv_addr[i].sin_family = AF_INET;
	serv_addr[i].sin_addr.s_addr = INADDR_ANY;
	serv_addr[i].sin_port = htons(udpPortList[i]);
	if (bind(sockfd[i], (struct sockaddr *) &serv_addr[i],sizeof(serv_addr[i])) < 0) 
	    printf("printf on binding");

	udpSocketDataObj = (struct udpSocketData *)malloc(sizeof(struct udpSocketData));
	udpSocketDataObj->sockfd = sockfd[i];
	udpSocketDataObj->myId = i;
	udpSocketDataObj->serv_addr = serv_addr[i];

	listOfThreads.push_back(udpWriteThread[i]);
	listOfThreads.push_back(udpReadThread[i]);
	listOfSockfd.push_back(sockfd[i]);

	pthread_create(&udpWriteThread[i], NULL, UDPwriteThread, (void *)udpSocketDataObj);
	pthread_create(&udpReadThread[i], NULL, UDPreadThread, (void *)udpSocketDataObj);
	memset(&udpSocketDataObj, 0, sizeof(udpSocketDataObj));
    }


    for(int i = 0;i < NUM_UDP_CONNECTION;i++){
	pthread_join(udpWriteThread[i], NULL);
	pthread_join(udpReadThread[i], NULL);
    }
    return 0;

}

// This thread is invoked after a UDP connection has 
// been established. It is responsible to receive all
// the file content, write to file, and update the
// bitvector
void *UDPreadThread(void *temp){

    struct sockaddr_in from;
    struct udpSocketData *udpSocketDataObj = (struct udpSocketData *)temp;
    int fromlen = 0, n;
    dropPacketCount = 0 ;

    fromlen = sizeof(struct sockaddr_in);
    unsigned char tempBuf[16] ;
    memset(tempBuf, '\0', 16) ;

    while (!shutDownFlag){
    udpMessage mes;

	n = recvfrom(udpSocketDataObj->sockfd,&mes,sizeof(mes),0,(struct sockaddr *)&from,(socklen_t *)&fromlen);
	if (n < 0 || shutDownFlag){
//	    printf("Error: print on recvfrom");
	    if(shutDownFlag)
		pthread_exit(0);
	}

	if (readBit(bitV, mes.sequenceNum) == 0x00){
	    writeBit(bitV, mes.sequenceNum, 0x01) ;
//	    if(mes.data_len < MAXDATASIZE){
//		printf("%d is screwed\n", mes.sequenceNum) ;
//	    }
//	    printf("Packet %d - %d received by %d\n",mes.sequenceNum,mes.data_len, udpSocketDataObj->sockfd);
	    //	    for(int i = 0 ; i < mes.data_len - 26 ; i=i+16){
//		if(memcmp(&mes.buffer[i], tempBuf, 16) == 0){
//		    printf("Screwed\n") ;
//		}
//	    }
	    pthread_mutex_lock(&udpMessageClientQLock) ;
	    // push the unsigned char in Q
	    udpMessageClientQ.push_back(mes) ;
	    // Signal the write thread to wake up
	    pthread_cond_signal(&udpMessageClientQCV) ;
	    // release the lock
	    pthread_mutex_unlock(&udpMessageClientQLock) ;
	    // Set the bit vector
	    ++packetsRcvd ;
	}
	else{
	    ++dropPacketCount;
//	    printf("Duplicate %d\n", mes.sequenceNum) ;
	}
    }
    close(udpSocketDataObj->sockfd);
    pthread_exit(0);
}

// This thread might not be of any use, so we wont
// implement it now
void *UDPwriteThread(void *){

//    printf("In the client's UDP write mode.....\n");
    pthread_exit(0);
}
