#include "client.h"


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int connetToServer(struct addrinfo* &p, int &sockfd){
    int rv;
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(objParam.serverName.c_str(), TCP_PORT, &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return -1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
	if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
	    perror("client: socket");
	    continue;
	}

	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd);
	    perror("client: connect");
	    continue;
	}
	break;
    }

    if (p == NULL) {
	fprintf(stderr, "client: failed to connect\n");
	return -1;
    }

    freeaddrinfo(servinfo); // all done with this structure
    return 1;

}

void *TCPconnectionThread(void *arg){
    int sockfd = 0;  
    struct addrinfo *p;
    char s[INET6_ADDRSTRLEN];

    if( connetToServer(p, sockfd) < 0 )
	pthread_exit(0);

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("Client: connecting to %s\n", s);

    // Initiate the TCP read thread
    pthread_t tcpReadThread;
    int res = pthread_create(&tcpReadThread, NULL, TCPreadThread, (void *)sockfd); 
    if( res != 0){
	fprintf(stderr, "TCP read thread creation failed\n") ;
	exit(EXIT_FAILURE) ;
    }

    // Initiate the TCP write thread
    pthread_t tcpWriteThread;
    res = pthread_create(&tcpWriteThread, NULL, TCPwriteThread, (void *)sockfd); 
    if( res != 0){
	fprintf(stderr, "TCP write thread creation failed\n") ;
	exit(EXIT_FAILURE) ;
    }

    // SEND file name to the server using TCP connection
    pushMessageInTCPq(0x1a,(unsigned char *)objParam.serverFilePath.c_str(), (int)objParam.serverFilePath.size());

    listOfThreads.push_back(tcpReadThread);
    listOfThreads.push_back(tcpWriteThread);
    listOfSockfd.push_back(sockfd);
    pthread_join(tcpReadThread, NULL);
    pthread_join(tcpWriteThread, NULL);
    //close(sockfd);
    pthread_exit(0);
}

void processReceivedTCPmessage(uint8_t message_type, unsigned char *buffer, uint32_t data_len){
    if(message_type == 0x1b){
	handleFileInfo(buffer, data_len) ;
    }
    else if(message_type == 0x1c){
	handleFileNotFound(buffer, data_len) ;
    }
    else if(message_type == 0x2a){
	handleAckRequest(buffer, data_len) ;
    }
    else{
	fprintf(stderr, "TCP message not recognized\n") ;
    }
}

void handleFileInfo(unsigned char *buffer, uint32_t data_len){
    buffer[data_len] = '\0';
    int rv=0;
    objParam.fileSize = atol((char *)buffer);
    if(objParam.fileSize % MAXDATASIZE == 0 )
	    objParam.noOfSeq = objParam.fileSize/MAXDATASIZE;
    else
    	objParam.noOfSeq = (objParam.fileSize/MAXDATASIZE) + 1;
  
    cout << "no of packets to receive from the server " << objParam.noOfSeq << endl;
    // Allocate memory to bitV
    printf("Initialising the bitvector..\n") ;
    bitV = (unsigned char *)malloc(objParam.noOfSeq/8+1) ;
    memset(bitV, 0x00, objParam.noOfSeq/8 + 1) ;

    pthread_t writeToFileThread;
    int res = pthread_create(&writeToFileThread, NULL, WriteToFileThread, &rv); 
    if( res != 0){
	fprintf(stderr, "UDP Connection thread creation failed\n") ;
	exit(EXIT_FAILURE) ;
    }
}

void handleFileNotFound(unsigned char *buffer, uint32_t data_len){
    cout << "File not found at the server\n";
    // Call the sshutdown method here
}

void handleAckRequest(unsigned char *buffer, uint32_t data_len){
    uint64_t seq_num, last_seq_num ;
    memcpy(&seq_num, buffer, 8) ;
    memcpy(&last_seq_num, buffer+8, 8) ;
//    printf("Server needs a ACK list %d - %d\n", seq_num, last_seq_num) ;
    // get bitvector
    int part_bv_len = (last_seq_num - seq_num)/8 + 17 ;
    unsigned char *part_bv = (unsigned char *)malloc(part_bv_len) ;
    if(last_seq_num != objParam.noOfSeq - 1)
	usleep(10) ;
    getBitVector(bitV, seq_num, last_seq_num, part_bv+16) ;
    memcpy(part_bv, &seq_num, 8) ;
    memcpy(part_bv+8, &last_seq_num, 8) ;

    pushMessageInTCPq(0x2b, part_bv, part_bv_len) ; 
}
