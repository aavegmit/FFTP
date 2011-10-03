#include "server.h"
#include <sys/time.h>

int main(int argc, char **argv){

    int rv = 0;
//    noOfPacketsSent = 0 ;
//    uptoPacketSent = -1 ;
    noOfLossPackets = 0 ;

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

    myPrepareData *m = ((myPrepareData *)args);
    //int myId = temp;
    uint64_t sequenceNum;
    uint32_t size = 0;
    unsigned char *fileData = (unsigned char *)malloc(MAXDATASIZE+1);
    bool alreadySet = false;
    udpMessage mes;
    memset(fileData, '\0',MAXDATASIZE+1);
    uptoPacketSent[m->myId] = m->startSeqNum - 1;
    uint64_t startSeqForAck = m->startSeqNum - 1;
    noOfAckSent[m->myId] = 0 ;
    noOfAckRecd[m->myId] = 0 ;
    if (m->myId ==  0)
	uptoPacketSent[0] = -1 ;
    noOfPacketsSent[m->myId] = 0;


    printf("PrepareThread...%d, %llu\n", m->myId, m->startSeqNum);
    while(!shutDownFlag){
        pthread_mutex_lock(&sequenceNumberListLock[m->myId]);
        if(sequenceNumberList[m->myId].size() == 0){
            if(m->startSeqNum <= m->endSeqNum){
                sequenceNum = m->startSeqNum++;
                alreadySet = true;
            }
	    else{
		if(noOfAckSent[m->myId] == noOfAckRecd[m->myId]){
		    if(uptoPacketSent[m->myId] == m->endSeqNum){
			printf("%d is done\n", m->myId) ;
		    }
		    if(uptoPacketSent[m->myId] == startSeqForAck )
			sendAckRequest(uptoPacketSent[m->myId]+1, m->startSeqNum - 1) ;
		    else
			sendAckRequest(uptoPacketSent[m->myId], m->startSeqNum - 1) ;
		    ++noOfAckSent[m->myId] ;
		}
//		printf("Going on wait %d\n", m->myId) ;
                pthread_cond_wait(&sequenceNumberListCV[m->myId], &sequenceNumberListLock[m->myId]);
            }
            if(shutDownFlag){
                pthread_mutex_unlock(&sequenceNumberListLock[m->myId]);
                pthread_exit(0) ;
            }
        }
        if(!alreadySet){
            sequenceNum = sequenceNumberList[m->myId].front();
            sequenceNumberList[m->myId].pop_front();
	    writeBit(toBeSendV, sequenceNum, 0x00) ;
        }
        pthread_mutex_unlock(&sequenceNumberListLock[m->myId]);
        //    	printf("%d out of seq list\n", sequenceNum) ;


//	printf("Sending %d\n", sequenceNum) ;
        //first check in cache
        if(inUDPpacketCache(sequenceNum)){
            pthread_mutex_lock(&udpPacketCacheLock);
            mes = udpPacketCache[sequenceNum];
            memcpy(fileData, mes.buffer, mes.data_len);
	    printf("Reading from cache - %d %d\n", mes.sequenceNum, mes.data_len) ;
	    size = mes.data_len ;
            pthread_mutex_unlock(&udpPacketCacheLock);
        }
        else{
            //READ from mmap
            getDataFromFile(sequenceNum, fileData, &size);

            //skipped compression

            //creating a packet from fileData
//            mes = getUDPpacketFromData(sequenceNum, size, fileData);

            //write to cache skipped
            //printf("Writing in Cache: %d\n", sequenceNum);
            writeToCache(sequenceNum, mes, RANDOM_PACKET);	
        }

        pushMessageInUDPq(sequenceNum, size, fileData, m->myId);
        // Increase the number of packets for ACK handling
        pthread_mutex_lock(&packetSentLock) ;
	++noOfPacketsSent[m->myId] ;
        if(shouldSendAck(noOfPacketsSent[m->myId], m->myId)){
	    if(uptoPacketSent[m->myId] == startSeqForAck )
		sendAckRequest(uptoPacketSent[m->myId]+1, m->startSeqNum - 1) ;
	    else
		sendAckRequest(uptoPacketSent[m->myId], m->startSeqNum - 1) ;
	    
	    ++noOfAckSent[m->myId] ;
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

    //creating a packet from fileData
    udpMessage mes = getUDPpacketFromData(sequenceNum, size, fileData);
    free(fileData) ;
    return mes;
}

bool shouldSendAck(long numPacketsSent, int sender_num){
    if(noOfAckSent[sender_num] != noOfAckRecd[sender_num]){
	return false;
    }
    if(numPacketsSent < objParam.noOfSeq/NUM_UDP_CONNECTION){
	if (numPacketsSent % (objParam.noOfSeq / (ACK_SENDING_RATIO*NUM_UDP_CONNECTION)) == 0){
	    return true;
	}
    }
    else if(numPacketsSent == objParam.noOfSeq/NUM_UDP_CONNECTION)
	return true;
    else{
	if((numPacketsSent - objParam.noOfSeq/NUM_UDP_CONNECTION) % CRITICAL_ACK_SENDING_GAP == 0){
	    return true;
	}
    }
    return false;
}
