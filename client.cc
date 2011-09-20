#include "client.h"

int main(int argc, char **argv){
	
	int rv = 0;
	// Call the command line parsing function
	

	// THREAD - tcp client
	
	// Create NumberUDPConnection of udp connections
	pthread_t tcpConnectionThread;
	pthread_create(&tcpConnectionThread, NULL, TCPconnectionThread, &rv); 
	// Thread - udp client
	

	// Wait for all the threads to stop
	pthread_join(tcpConnectionThread, NULL);
}

