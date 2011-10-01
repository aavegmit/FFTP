#include "server.h"

long noOfPacketsSent;
long uptoPacketSent;
long noOfAckSent;
long noOfAckRecd;

int main(int argc, char **argv){

    int rv = 0;
    noOfPacketsSent = 0 ;
    uptoPacketSent = -1 ;
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

    myPrepareData m = *((myPrepareData *)args);
    uint64_t sequenceNum;
    uint32_t size = 0;
    unsigned char *fileData = (unsigned char *)malloc(MAXDATASIZE+1);
    bool alreadySet = false;
    udpMessage mes;
    memset(fileData, '\0',MAXDATASIZE+1);
    long lastSeqNumForAck = 0;

    printf("PrepareThread...%d, %llu\n", m.myId, m.startSeqNum);
    while(!shutDownFlag){
        pthread_mutex_lock(&sequenceNumberListLock[m.myId]);
        if(sequenceNumberList[m.myId].size() == 0){
            if(m.startSeqNum < m.endSeqNum){
                sequenceNum = m.startSeqNum++;
                alreadySet = true;
            }
            else{
                if(noOfAckSent == noOfAckRecd){
                    sendAckRequest(uptoPacketSent, m.startSeqNum) ;
                }
                pthread_cond_wait(&sequenceNumberListCV[m.myId], &sequenceNumberListLock[m.myId]);
            }
            if(shutDownFlag){
                pthread_mutex_unlock(&sequenceNumberListLock[m.myId]);
                pthread_exit(0) ;
            }
        }
        if(!alreadySet){
            sequenceNum = sequenceNumberList[m.myId].front();
            sequenceNumberList[m.myId].pop_front();
	    toBeSend.erase(sequenceNum) ;
        }
        pthread_mutex_unlock(&sequenceNumberListLock[m.myId]);
    //    	printf("%d out of seq list\n", sequenceNum) ;


        //first check in cache
        //        if(inUDPpacketCache(sequenceNum)){
        //            pthread_mutex_lock(&udpPacketCacheLock);
        //            mes = udpPacketCache[sequenceNum];
        //            memcpy(fileData, mes.buffer, mes.data_len);
        //            pthread_mutex_unlock(&udpPacketCacheLock);
        //        }
        //        else{
        //READ from mmap
        getDataFromFile(sequenceNum, fileData, &size);

        //skipped compression

        //creating a packet from fileData
        mes = getUDPpacketFromData(sequenceNum, size, fileData);

        //write to cache skipped
        //            writeToCache(sequenceNum, mes, RANDOM_PACKET);	
        //        }

        pushMessageInUDPq(sequenceNum, size, fileData, m.myId);
        // Increase the number of packets for ACK handling
        pthread_mutex_lock(&packetSentLock) ;
        ++noOfPacketsSent ;
        if(shouldSendAck(noOfPacketsSent)){
            sendAckRequest(uptoPacketSent, m.startSeqNum) ;
        }
        pthread_mutex_unlock(&packetSentLock) ;

        memset(fileData, '\0',MAXDATASIZE+1);
        alreadySet = false;
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
    if(size > MAXDATASIZE)
	printf("Yups, size me chodh hai..%d on seq %d\n", size, sequenceNum) ;

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
            if((numPacketsSent - objParam.noOfSeq) % CRITICAL_ACK_SENDING_GAP == 0){
                return true;
            }
        }
        return false;
    }


void displayStats(){
    printf("Displaying stats..\n") ;
    printf("#Total transmissions %ld\n", noOfPacketsSent ) ;
    printf("#Udp server Wait count\n") ;
    for(int i = 0 ; i < NUM_UDP_CONNECTION ; ++i){
        printf("\tUdp server %d - %ld\n",i, udpSendWaitCount[i]) ; 
    }
}
