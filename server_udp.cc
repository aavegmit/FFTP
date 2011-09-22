#include "server.h"

//extern variable declared in server.h
int udpPortList[5] = {42000, 42001, 42002, 42003, 42004};
unsigned char clientName[] = CLIENT_NAME;
list<unsigned char* > fileDataList;
pthread_mutex_t udpWriteThreadLock;
pthread_cond_t udpWriteThreadCV;

void *UDPserverThread(void *){

    int sockfd[NUM_UDP_CONNECTION];
    struct sockaddr_in serv_addr[NUM_UDP_CONNECTION];
    struct hostent *server;
    //struct sockaddr_storage from;
    struct udpSocketData *udpSocketDataObj;
    pthread_t udpWriteThread[NUM_UDP_CONNECTION];
    pthread_t udpReadThread[NUM_UDP_CONNECTION];

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


//Read thread for UDP server
void *UDPreadThread(void *udpSocketDataObj){

	printf("In the UDP read thread.....\n");
	return 0;
}

//Write Thread of UDP server
void *UDPwriteThread(void *temp){

struct udpSocketData *udpSocketDataObj = (struct udpSocketData *)temp;
unsigned char *msgBuffer;
int n;

	pthread_mutex_init(&udpWriteThreadLock, NULL);
	pthread_cond_init(&udpWriteThreadCV,NULL);

	printf("Sending UDP packets to client.....\n");
	fileDataList.push_back((unsigned char *)"Hello");
	fileDataList.push_back((unsigned char *)"World");
	fileDataList.push_back((unsigned char *)"!!!!!");

	while(1){
		
		if(fileDataList.empty()){

			printf("Nothing in message Queue, going on wait\n");
			pthread_mutex_lock(&udpWriteThreadLock);
			pthread_cond_wait(&udpWriteThreadCV, &udpWriteThreadLock);
			pthread_mutex_unlock(&udpWriteThreadLock);
		}
		
		msgBuffer = fileDataList.front();
		fileDataList.pop_front();
		n = sendto(udpSocketDataObj->sockfd, msgBuffer, strlen((char *)msgBuffer), 0,(struct sockaddr *) &udpSocketDataObj->serv_addr,sizeof(udpSocketDataObj->serv_addr));
		if (n < 0) {
			printf("ERROR writing to UDP socket\n");
			exit(0);
		}
	}
	return 0;
}
