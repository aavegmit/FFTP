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

    int sockfd = 0, numbytes;  
    struct addrinfo *p;
    char buf[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];
    
    if( connetToServer(p, sockfd) < 0 )
	pthread_exit(0);

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        pthread_exit(0);
    }

    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);
    close(sockfd);
 
    pthread_exit(0);
}


void *TCPreadThread(void *){

    pthread_exit(0);   
}


void *TCPwriteThread(void *){

    pthread_exit(0);
}
