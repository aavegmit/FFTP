#include "client.h"

param objParam;

int parseCommandLine(char **argv){
    objParam.serverName = argv[1];
    objParam.serverFilePath = argv[1];
    objParam.localFilePath = argv[2];
    int pos = (int) objParam.serverName.find(":");	
    if (pos==string::npos)
	return -1;

    objParam.serverName = objParam.serverName.substr(0,pos);
    objParam.serverFilePath = objParam.serverFilePath.substr(pos+1);
//	cout << objParam.serverName << "    " << objParam.serverFilePath << endl;
    return 1;
}

int main(int argc, char **argv){
	int rv = 0;
	
	if( argc != 3 ){
	    cout << "Malformed command: \n";
	    return 0;
	}
	
	if( parseCommandLine(argv) < 0 )
	    return 0;		

	init() ;

	
	// Create NumberUDPConnection of udp connections
	pthread_t tcpConnectionThread;
	int res = pthread_create(&tcpConnectionThread, NULL, TCPconnectionThread, &rv); 
	if( res != 0){
		fprintf(stderr, "TCP Connection thread creation failed\n") ;
		exit(EXIT_FAILURE) ;
	}

	// Thread - udp client
//	pthread_t udpConnectionThread;
//	res = pthread_create(&udpConnectionThread, NULL, UDPconnectionThread, &rv); 
//	if( res != 0){
//		fprintf(stderr, "UDP Connection thread creation failed\n") ;
//		exit(EXIT_FAILURE) ;
//	}

	// Wait for all the threads to stop
	pthread_join(tcpConnectionThread, NULL);
//	pthread_join(udpConnectionThread, NULL);
	printf("Client Exiting.....\n");
}

