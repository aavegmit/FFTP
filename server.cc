#include "server.h"

int main(int argc, char **argv){

    int rv = 0;

    init() ;

    // Thread - Start TCP server
    pthread_t tcpServerThread;	
    pthread_create(&tcpServerThread, NULL, TCPserverThread, &rv);

    //Thread - Start UDP Server
    pthread_t udpServerThread;	
    pthread_create(&udpServerThread, NULL, UDPserverThread, &rv);

    // Wait for the TCP server thread to close
    pthread_join(tcpServerThread, NULL);	
    //pthread_join(udpServerThread, NULL);	


}

// Fetches the unsent/lost blocks from the cache/file
// and puts them in the server UDP write queue
void *prepareBlockThread(void *args){
    uint64_t sequenceNum;
    uint32_t size = 0;
    unsigned char *fileData = (unsigned char *)malloc(MAXDATASIZE+1);
    udpMessage mes;
    memset(fileData, '\0',MAXDATASIZE+1);

    loadFileToMMap();
    populateSequenceNumberList();
    //printMMapToFile();
    while(1){

        pthread_mutex_lock(&sequenceNumberListLock);
        if(sequenceNumberList.size() == 0){
            
            pthread_cond_wait(&sequenceNumberListCV, &sequenceNumberListLock);
            pthread_mutex_unlock(&sequenceNumberListLock);

        }
        else
            pthread_mutex_unlock(&sequenceNumberListLock);

        //reading from the start of sequence number list
        pthread_mutex_lock(&sequenceNumberListLock);
        sequenceNum = sequenceNumberList.front();
        sequenceNumberList.pop_front();
        pthread_mutex_unlock(&sequenceNumberListLock);

        //first check in cache
        if(inUDPpacketCache(sequenceNum)){
            printf("Found packet in cache...\n");
            pthread_mutex_lock(&udpPacketCacheLock);
            mes = udpPacketCache[sequenceNum];
            pthread_mutex_unlock(&udpPacketCacheLock);
        }
        else{
            //READ from mmap
            getDataFromFile(sequenceNum, fileData, &size);

            //creating a packet from fileData
            mes = getUDPpacketFromData(sequenceNum, size, fileData);

            //skipped compression

            //write to cache skipped
            writeToCache(sequenceNum, mes, RANDOM_PACKET);	
        }

        //sleep(1);
        //putting the 'fileData' into the list
        //printf("Packet sent to UDP Message Q....\n");
        pushMessageInUDPq(sequenceNum, size, fileData);

        memset(fileData, '\0',MAXDATASIZE+1);
    }

    //	fclose(f);
    unloadFileMap();
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
