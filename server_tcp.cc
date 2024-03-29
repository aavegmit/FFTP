#include "server.h"

// get sockaddr IPV4 or IPV6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int listenServer(int &sockfd){
    if (listen(sockfd, TCP_BACKLOG) == -1) {
        perror("listen");
        return -1;
    }
    return 1;
}

int bindServerInfo(int &sockfd){

    struct addrinfo hints, *servinfo, *p;
    int rv = 0, yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, TCP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                    sizeof(int)) == -1) {
            perror("setsockopt");
            return -1; 
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return -1;
    }

    freeaddrinfo(servinfo); // all done with this structure
    return 1;

} 

int acceptRequest(struct sockaddr_storage &their_addr, int &sockfd, int &new_fd){
    socklen_t sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
        return -1;
    }
    return 1;
}

void *TCPserverThread(void *arg){
    int sockfd, new_fd, rv=0;  
    struct sockaddr_storage their_addr; 
    char s[INET6_ADDRSTRLEN];


    if( bindServerInfo(sockfd) < 0 )
        pthread_exit(0);

    if( listenServer(sockfd) < 0 )
        pthread_exit(0);	

    printf("server: waiting for connections...\n");

    pthread_t tcpReadThread;
    pthread_t tcpWriteThread;
    while(1) {  // main accept() loop
        new_fd = 0;	
        if( acceptRequest(their_addr, sockfd, new_fd) < 0 )
            continue;

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
        memcpy(clientName, s, sizeof(s));

        if (new_fd < 0){
            break ;
            //	    close(new_fd) ;
        }
        else
        { 
            //	    close(sockfd); // child doesn't need the listener
            // Initiate the TCP read thread
            int res = pthread_create(&tcpReadThread, NULL, TCPreadThread, (void *)new_fd); 
            if( res != 0){
                fprintf(stderr, "TCP read thread creation failed\n") ;
                exit(EXIT_FAILURE) ;
            }

            // Initiate the TCP write thread
            res = pthread_create(&tcpWriteThread, NULL, TCPwriteThread, (void *)new_fd); 
            if( res != 0){
                fprintf(stderr, "TCP write thread creation failed\n") ;
                exit(EXIT_FAILURE) ;
            }
        }
    }
    pthread_join(tcpReadThread, NULL) ;
    pthread_join(tcpWriteThread, NULL) ;
    pthread_exit(0);
}

void processReceivedTCPmessage(uint8_t message_type, unsigned char *buffer, uint32_t data_len){
    if(message_type == 0x1a){
        handleFileName(buffer, data_len) ;
    }
    else if(message_type == 0x2b){
        handleACKlist(buffer, data_len) ;
    }
    else if(message_type == 0xff){
        startUDPServerThreads();
    }
    else if(message_type == 0xee){
        displayStats();
        printf("Server Ends here....\n");
        exit(0);
    }
    else{
        fprintf(stderr, "TCP message not recognized\n") ;
    }
}

// Receives file name 
// initialize the thread - prepareBlockThread
void handleFileName(unsigned char *buffer, uint32_t data_len){
    fileInfoObj.fileName = string((char *)buffer, data_len) ;
    FILE *fp;
    int rv = 0 ;
    fp = fopen(fileInfoObj.fileName.c_str(),"r"); 
    if(fp==NULL)
    {
        cout << fileInfoObj.fileName << "The file doesn't exist " << endl;
        pushMessageInTCPq(0x1c, NULL, 0);
        return;
    }
    if( stat(fileInfoObj.fileName.c_str(), &fileInfoObj.fileStat) < 0)
        return;	 
    fclose(fp); 
    std::stringstream ss;
    ss << fileInfoObj.fileStat.st_size << "\0";
    string pktSize = ss.str();
    pushMessageInTCPq(0x1b,(unsigned char *) pktSize.c_str(), pktSize.size());

    //	Thread writes the data to UDP write thread's list
    loadFileToMMap();
    //populateSequenceNumberList();
    int temp[NUM_UDP_CONNECTION];
    uint64_t q = (fileInfoObj.fileStat.st_size/MAXDATASIZE);
    uint64_t r = (fileInfoObj.fileStat.st_size%MAXDATASIZE);

    if(r > 0)
        q+=1;
//    printf("Q IS: %llu\n", q);
    toBeSendV = (unsigned char *)malloc(q/8+2) ;
    memset(toBeSendV, 0x00, q/8+2) ;
    objParam.noOfSeq = q ;

    r = q%NUM_UDP_CONNECTION;
    q = q/NUM_UDP_CONNECTION;

    for(int i = 0;i<NUM_UDP_CONNECTION;i++){

        myPrepareData *m = (myPrepareData *)malloc(sizeof(myPrepareData));
        pthread_t PrepareBlockThread;
        //temp[i] = i;
        m->myId = i;
        m->startSeqNum = q*i;
        if(i == NUM_UDP_CONNECTION-1)
            m->endSeqNum = (q*(i+1) - 1) + r;
        else
            m->endSeqNum = (q*(i+1) - 1) ;

//        printf("PrepareBlockThread is creating: %d, startSeq: %llu, endSeqNum: %llu\n", m->myId, m->startSeqNum, m->endSeqNum);
        pthread_create(&PrepareBlockThread, NULL, prepareBlockThread, (void *)m);
    }
}

// Sets the bit vector
// If a packet is lost, then add it in the  FRONT of the list of
// sequence numbers
// Write to cache
// Clear out the cache for which packets has been received
void handleACKlist(unsigned char *buffer, uint32_t data_len){
    list<uint64_t> lossPackets ;
    int groupPush = 0 ;
    long localLossCount=0 ;
    uint64_t seq_num, last_seq_num, current_seq_num ;
    memcpy(&seq_num, buffer, 8) ;
    memcpy(&last_seq_num, buffer + 8, 8) ;
    int sender_num = seq_num / ((int)(objParam.noOfSeq / NUM_UDP_CONNECTION )) ;
    if(sender_num >= NUM_UDP_CONNECTION)
	sender_num = NUM_UDP_CONNECTION - 1 ;
        printf("Server receives a ACK list (%d) from the client...%llu - %llu %d\n", sender_num ,seq_num, last_seq_num ,data_len) ;
    uint64_t seq_num_mod = (seq_num/8)*8 ;
    for(uint64_t i = (seq_num%8) ; i <= last_seq_num - seq_num_mod ; ++i){
        current_seq_num = seq_num_mod + i ;
	if(readBit(buffer+16, i) == 0x01){
	    printf("Packet received - %ld, upto %ld\n", current_seq_num, uptoPacketSent[sender_num]) ;
	    removeFromUDPPacketCache(current_seq_num) ;
	    if (current_seq_num == uptoPacketSent[sender_num] + 1)
                uptoPacketSent[sender_num] = current_seq_num ;
        }
	else{
	    //	    if(toBeSend.find(current_seq_num) == toBeSend.end()){

	    if(readBit(toBeSendV, current_seq_num) == 0x00){
		udpMessage mes =  getUDPpacketFromSeqNum(current_seq_num) ;
		//		writeToCache(current_seq_num, mes, LOST_PACKET ) ;

		printf("Packet lost - %d\n", current_seq_num) ;
		//		toBeSend[current_seq_num] = true ;

		writeBit(toBeSendV, current_seq_num, 0x01) ;
		//		if(groupPush == 0 || groupPush % 10 != 0 || groupPush != 1){
		lossPackets.push_back(current_seq_num) ;
		//		}
		//		else{
		//		    pushSequenceNumberInList(current_seq_num) ;
		//		}
		//		++groupPush ;
		//		if(groupPush % 4 == 0)
		//		    lossPackets.push_front(current_seq_num) ;
		++localLossCount;
		++noOfLossPackets ;
	    }
	}
	}

	printf("LOSS COUNT %ld\t %d%\n", localLossCount, localLossCount*100/(last_seq_num - seq_num)) ;
	if(localLossCount*100/(last_seq_num - seq_num) > LOSS_RATIO + LOSS_LIMIT)
	    slowDown = true ;
	else
	    slowDown = false ;

    uint64_t lossSeqNo ;
    pthread_mutex_lock(&sequenceNumberListLock[sender_num]);
    if(lossPackets.size() != 0){
	while(lossPackets.size() > 0){
	    lossSeqNo = lossPackets.front() ;
	    sequenceNumberList[sender_num].push_back(lossSeqNo);
	    lossPackets.pop_front() ;
	}
	pthread_cond_signal(&sequenceNumberListCV[sender_num]);
    }
    ++noOfAckRecd[sender_num] ;
    pthread_mutex_unlock(&sequenceNumberListLock[sender_num]);
//    pushSequenceNumberListInList(lossPackets) ;
}

void startUDPServerThreads(){

int rv = 0;
    printf("Starting UDP Threads...\n");
    //Thread - Start UDP Server
    pthread_t udpServerThread;
    int res = pthread_create(&udpServerThread, NULL, UDPserverThread, &rv);
    if( res != 0){
        fprintf(stderr, "TCP read thread creation failed\n") ;
        exit(EXIT_FAILURE) ;
    }
}

void sendAckRequest(int64_t seq_num, uint64_t last_seq_num){
    if(seq_num == -1)
        seq_num = 0 ;
//    printf("Sending Ack request %d - %d\n", seq_num, last_seq_num) ;
    unsigned char *buffer = (unsigned char *)malloc(16) ;
    if (last_seq_num > seq_num + 10400)
        last_seq_num = seq_num + 10400 ;
    memcpy(buffer, &seq_num, 8) ;
    memcpy(buffer+8, &last_seq_num, 8);
    pushMessageInTCPq(0x2a, buffer, 16);
    free(buffer) ;
}
