#include "server.h"

long noOfPacketsSent;
long uptoPacketSent;
long lastSeqNumForAck;
long noOfAckSent;
long noOfAckRecd;
map<long, bool> toBeSend;

int main(int argc, char **argv){

    int rv = 0;
    noOfPacketsSent = 0 ;
    uptoPacketSent = -1 ;
    lastSeqNumForAck = 0 ;
    noOfAckSent = 0 ;
    noOfAckRecd = 0 ;

    init() ;

    // Thread - Start TCP server
    pthread_t tcpServerThread;	
    pthread_create(&tcpServerThread, NULL, TCPserverThread, &rv);

    // Wait for the TCP server thread to close
    pthread_join(tcpServerThread, NULL);	
}

// Fetches the unsent/lost blocks from the cache/file
// and puts them in the server UDP write queue
void *prepareBlockThread(void *args){

    int myId = *((int *)args);
    //int myId = temp;
    uint64_t sequenceNum;
    uint32_t size = 0;
    unsigned char *fileData = (unsigned char *)malloc(MAXDATASIZE+1);
    udpMessage mes;
    memset(fileData, '\0',MAXDATASIZE+1);

    //loadFileToMMap();
    //populateSequenceNumberList();
    while(1){

        pthread_mutex_lock(&sequenceNumberListLock);
        while(sequenceNumberList.size() == 0){
            printf("Going on wait...\n");
	    sendAckRequest(uptoPacketSent, lastSeqNumForAck) ;
            pthread_cond_wait(&sequenceNumberListCV, &sequenceNumberListLock);
        }
        sequenceNum = sequenceNumberList.front();
        sequenceNumberList.pop_front();
        pthread_mutex_unlock(&sequenceNumberListLock);
//	printf("%d out of seq list\n", sequenceNum) ;

	// Increase the number of packets for ACK handling
	++noOfPacketsSent ;
	if(shouldSendAck(noOfPacketsSent)){
	    sendAckRequest(uptoPacketSent, lastSeqNumForAck) ;
	}
	

        //first check in cache
        if(inUDPpacketCache(sequenceNum)){
//            printf("Found packet in cache...\n");
            pthread_mutex_lock(&udpPacketCacheLock);
            mes = udpPacketCache[sequenceNum];
            memcpy(fileData, mes.buffer, mes.data_len);
            pthread_mutex_unlock(&udpPacketCacheLock);
        }
        else{
            //READ from mmap
            getDataFromFile(sequenceNum, fileData, &size);
            
            //skipped compression

            //creating a packet from fileData
            mes = getUDPpacketFromData(sequenceNum, size, fileData);

            //write to cache skipped
            writeToCache(sequenceNum, mes, RANDOM_PACKET);	
        }

        pushMessageInUDPq(sequenceNum, size, fileData, myId);
	if(lastSeqNumForAck < sequenceNum)
	    lastSeqNumForAck = sequenceNum ;

        memset(fileData, '\0',MAXDATASIZE+1);
    }

    //NEED TO UNLOAD MAP SOMEWHERE
    //unloadFileMap();
    return 0;
}

udpMessage getUDPpacketFromSeqNum(uint64_t sequenceNum){
    uint32_t size = 0;
    unsigned char *fileData = (unsigned char *)malloc(MAXDATASIZE+1);
    //READ from mmap
    getDataFromFile(sequenceNum, fileData, &size);

    //creating a packet from fileData
    udpMessage mes = getUDPpacketFromData(sequenceNum, size, fileData);
    free(fileData) ;
    return mes;
}

bool shouldSendAck(long numPacketsSent){
    if(noOfAckSent != noOfAckRecd)
	return false;
    if(numPacketsSent < objParam.noOfSeq){
	if (numPacketsSent % (objParam.noOfSeq / ACK_SENDING_RATIO) == 0)
	    return true;
    }
    else if(numPacketsSent == objParam.noOfSeq)
	return true;
    else{
	if((numPacketsSent - objParam.noOfSeq) % CRITICAL_ACK_SENDING_GAP == 0)
	    return true;
    }
    return false;
}
