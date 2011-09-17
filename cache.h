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

// Data structure
// Map<int sequenceNumber, char *block>
// queue<int sequenceNumber>



writeToCache(int sequenceNumber, char *block) ;
// Save in Cache 
// 	- We save two type of blocks, random blocks or the ones which has been lost
//	- If cache full, and the block to be inserted is "random", then dont save it
//	- If cache full, and the block to be inserted is "lost", then remove one "random" block and save this one
//		- We can maintain a list/queue of "random" blocks sequence number so as to expedite the 
//		  search of a random block
//	- If cache full, and the block to be inserted is "lost", and there is no "random" block in the cache,
//	  then dont save but print that the cache is full so we know that the cache size has to be tweaked
