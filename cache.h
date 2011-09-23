#ifndef _CACHE_H
#define _CACHE_H

#include "shared.h"
#include <map>


/*
 * Group - 3-Lost-Packets
 *
 * This file includes the methods related to
 * the cache used for storing the blocks read
 * from the file for future use.
 *
 * The cache is implemented using the C++ MAP.
 * It stores the block of data indexed to their
 * offset in the file.
 *
 * We might totally remove this functionality
 * if MMAP works better.
 */

void writeToCache(uint64_t, udpMessage, bool ) ;
void addToCachedUDPRandomPacketList(uint64_t);
uint64_t getCachedUDPRandomPacketList();
bool inUDPpacketCache(uint64_t);
void removeFromUDPPacketCache(uint64_t);
extern map<uint64_t, udpMessage> udpPacketCache;
extern pthread_mutex_t udpPacketCacheLock;
extern list<uint64_t > cachedUDPRandomPacketList;
#endif
