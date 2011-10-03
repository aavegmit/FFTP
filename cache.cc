#include "cache.h"

map<uint64_t, udpMessage> udpPacketCache;
pthread_mutex_t udpPacketCacheLock;
list<uint64_t > cachedUDPRandomPacketList;

bool inUDPpacketCache(uint64_t sequenceNum){
    //map<uint64_t, udpMessage>::iterator it;

    //it = udpPacketCache.find(sequenceNum);
    if(udpPacketCache.find(sequenceNum) == udpPacketCache.end()){
	    return false;
    }
    else
    	return true;
}

void addToCachedUDPRandomPacketList(uint64_t sequenceNum){
    cachedUDPRandomPacketList.push_back(sequenceNum);
}

uint64_t getCachedUDPRandomPacket(){

    uint64_t sequenceNum = cachedUDPRandomPacketList.front();
    //cachedUDPRandomPacketList.pop_front();
    return sequenceNum;

}

void removeFromUDPPacketCache(uint64_t sequenceNum){
    pthread_mutex_lock(&udpPacketCacheLock);
    udpPacketCache.erase(sequenceNum);
    cachedUDPRandomPacketList.remove(sequenceNum);
    pthread_mutex_unlock(&udpPacketCacheLock);
}

// Save in Cache 
// 	- We save two type of blocks, random blocks or the ones which has been lost
//	- If cache full, and the block to be inserted is "random", then dont save it
//	- If cache full, and the block to be inserted is "lost", then remove one "random" block and save this one
//		- We can maintain a list/queue of "random" blocks sequence number so as to expedite the 
//		  search of a random block
//	- If cache full, and the block to be inserted is "lost", and there is no "random" block in the cache,
//	  then dont save but print that the cache is full so we know that the cache size has to be tweaked
void writeToCache(uint64_t sequenceNum, udpMessage mes, bool packetType){

    pthread_mutex_lock(&udpPacketCacheLock);

    //need to remove an element from the cache
    if(udpPacketCache.size() == MAX_CACHE_SIZE){

	if(packetType == LOST_PACKET){
	    if(cachedUDPRandomPacketList.size() == 0){
//		printf("Need more cache space....no random packets...only lost packets\n");
	    }
	    else{
		uint64_t tempSeqNum = getCachedUDPRandomPacket();
		udpPacketCache.erase(tempSeqNum);
		cachedUDPRandomPacketList.remove(tempSeqNum);
		udpPacketCache[sequenceNum] = mes;
	    }
	}
    }
    else{
	printf("Writing packet to cache...%llu, %lu\n", mes.sequenceNum, mes.data_len);
	udpPacketCache[sequenceNum] = mes;
	if(packetType == RANDOM_PACKET){
	    addToCachedUDPRandomPacketList(sequenceNum);
	}
    }

    pthread_mutex_unlock(&udpPacketCacheLock);
}
