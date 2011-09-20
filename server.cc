#include "server.h"



int main(int argc, char **argv){
	// Thread - Start UDP server 
	// Thread - Start TCP server


	// Wait for the TCP server thread to close
	// Wait for the UDP server thread to close
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

}
