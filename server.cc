#include "server.h"
#include "fileIO.h"
#include "cache.h"

int main(int argc, char **argv){

	int rv = 0;

	init() ;
	
	// Thread - Start TCP server
	//pthread_t tcpServerThread;	
	//pthread_create(&tcpServerThread, NULL, TCPserverThread, &rv);
	
	//Thread - Start UDP Server
	pthread_t udpServerThread;	
	pthread_create(&udpServerThread, NULL, UDPserverThread, &rv);
	
	//Thread writes the data to UDP write thread's list
	pthread_t PrepareBlockThread;
	pthread_create(&PrepareBlockThread, NULL, prepareBlockThread, &rv);

	// Wait for the TCP server thread to close
	//pthread_join(tcpServerThread, NULL);	
	pthread_join(udpServerThread, NULL);	
	pthread_join(PrepareBlockThread, NULL);
	

}

// Fetches the unsent/lost blocks from the cache/file
// and puts them in the server UDP write queue
void *prepareBlockThread(void *args){
	/* 
	 * For all the elements in the list
	 *    Take out the front element
	 *    if sequence number present in cache
 *          pushBlockInUDPWrite
	 *    else 
	 *    	    Read the block from file
	 *    	    Compress it 
	 *    	    write to cache
	 *    	    pushBlockInUDPWrite
	 *    end
	 * end
	 */
uint64_t sequenceNum;
uint32_t size = 0;
unsigned char *fileData = (unsigned char *)malloc(MAXDATASIZE+1);
udpMessage mes;
memset(fileData, '\0',MAXDATASIZE+1);

//	FILE *f = fopen("image.jpg", "wb");

	loadFileToMMap();
	populateSequenceNumberList();
	//printMMapToFile();
	while(!sequenceNumberList.empty()){
	 	//reading from the start of sequence number list
		pthread_mutex_lock(&sequenceNumberListLock);
		sequenceNum = sequenceNumberList.front();
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
		

		//putting the 'fileData' into the list
		//printf("Packet sent to UDP Message Q....\n");
		pushMessageInUDPq(sequenceNum, size, fileData);
		
		memset(fileData, '\0',MAXDATASIZE+1);
		pthread_mutex_lock(&sequenceNumberListLock);
		sequenceNumberList.pop_front();
		pthread_mutex_unlock(&sequenceNumberListLock);
	}
//	fclose(f);
	unloadFileMap();
	return 0;
}
