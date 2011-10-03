#include "server.h"


void *UDPserverThread(void *){

    int sockfd[NUM_UDP_CONNECTION];
    struct sockaddr_in serv_addr[NUM_UDP_CONNECTION];
    struct hostent *server;
    //struct sockaddr_storage from;
    struct udpSocketData *udpSocketDataObj;
    pthread_t udpMessageQ[NUM_UDP_CONNECTION];
    pthread_t udpReadThread[NUM_UDP_CONNECTION];

    initUDP();

    for(int i = 0 ; i < NUM_UDP_CONNECTION; i++){

        sockfd[i] = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd[i] < 0) {
            //error("ERROR opening UDP socket"); 
            printf("Error opening UDP socket\n");
            exit(0);
        }
    }

    server = gethostbyname((char *)clientName);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    for(int i = 0;i < NUM_UDP_CONNECTION; i++){
        bzero((char *) &serv_addr[i], sizeof(serv_addr[i]));
        serv_addr[i].sin_family = AF_INET;
        bcopy((char *)server->h_addr,(char *)&serv_addr[i].sin_addr.s_addr,server->h_length);
        serv_addr[i].sin_port = htons(udpPortList[i]);

        //populate UDP server data so to pass to read and write thread
        udpSocketDataObj = (struct udpSocketData *)malloc(sizeof(struct udpSocketData));
        udpSocketDataObj->serv_addr = serv_addr[i];
        udpSocketDataObj->sockfd = sockfd[i];
        udpSocketDataObj->myId = i;

        pthread_create(&udpMessageQ[i], NULL, UDPwriteThread, (void *)udpSocketDataObj);
        pthread_create(&udpReadThread[i], NULL, UDPreadThread, (void *)udpSocketDataObj);
        memset(&udpSocketDataObj, 0, sizeof(udpSocketDataObj));
    }

    for(int i = 0;i < NUM_UDP_CONNECTION;i++){
        pthread_join(udpMessageQ[i], NULL);
        pthread_join(udpReadThread[i], NULL);
    }
    return 0;
}


//Read thread for UDP server
void *UDPreadThread(void *udpSocketDataObj){

//    printf("In the UDP read thread.....\n");
    pthread_exit(0);
}

//Write Thread of UDP server
void *UDPwriteThread(void *temp){

    struct udpSocketData *udpSocketDataObj = (struct udpSocketData *)temp;
    int myId = udpSocketDataObj->myId;
    udpSendWaitCount[myId] = 0 ;
    udpMessage mes;
    int n;
    uint64_t counter = 0;
    while(1){
        pthread_mutex_lock(&udpMessageQLock[myId]);
        while(udpMessageQ[myId].empty()){
	    ++udpSendWaitCount[myId];
            pthread_cond_wait(&udpMessageQCV[myId], &udpMessageQLock[myId]);
        }

        mes = udpMessageQ[myId].front();
        udpMessageQ[myId].pop_front();
        pthread_mutex_unlock(&udpMessageQLock[myId]);

//        /*********************************/
//        if(counter % 20 == 0 && counter != 0)
//            usleep(10000);
//        /*******************************/
//
//        counter++;
//        printf("Sending %d UDP Packets to client....%d is sending : %llu\n",mes.sequenceNum, udpSocketDataObj->sockfd, counter);
//	if (random() % 100 < 60)
	    n = sendto(udpSocketDataObj->sockfd, &mes, sizeof(mes), 0,(struct sockaddr *) &udpSocketDataObj->serv_addr,sizeof(udpSocketDataObj->serv_addr));
//        if (n < 0) {
//            printf("ERROR writing to UDP socket\n");
//	    pthread_exit(0) ;
//        }
    }
    return 0;
}

