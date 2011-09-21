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
    int sockfd, new_fd;  
    struct sockaddr_storage their_addr; 
    char s[INET6_ADDRSTRLEN];
   
 
    if( bindServerInfo(sockfd) < 0 )
	pthread_exit(0);

    if( listenServer(sockfd) < 0 )
	pthread_exit(0);	

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
	new_fd = 0;	
	if( acceptRequest(their_addr, sockfd, new_fd) < 0 )
	    continue;

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
    
       if (!fork()) 
       { 
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    pthread_exit(0);

}

void processReceivedTCPmessage(uint8_t message_type, unsigned char *buffer, uint32_t data_len){
	if(message_type == 0x1a){
		handleFileName(buffer, data_len) ;
	}
	else if(message_type == 0x2b){
		handleACKlist(buffer, data_len) ;
	}
	else{
		fprintf(stderr, "TCP message not recognized\n") ;
	}
}

// Receives file name 
// initialize the bit vector with either all zero or one
// Creates a list of all the sequence numbers
// initialize the thread - prepareBlockThread
void handleFileName(unsigned char *buffer, uint32_t data_len){

}

// Sets the bit vector
// If a packet is lost, then add it in the  FRONT of the list of
// sequence numbers
// Write to cache
// Clear out the cache for which packets has been received
void handleACKlist(unsigned char *buffer, uint32_t data_len){

}

