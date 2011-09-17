#include <iostream>
#include <cstdio>

using namespace std ;

// Thread - start TCP server
//          Receives meta data
//          initialize the bit vector with either all zero or one
//          initialize the thread - File Input => Compress => Put in queue
// Thread - start UDP server


// Read thread function for both the threads may be common(not sure)
// Thread - Read thread for TCP server
// Thread - Read thread for UDP server


// Write thread function for both the threads may be common(not sure)
// Thread - Write thread for TCP server
// Thread - Write thread for UDP server


// Thread - Reads blocks from the file
// 	  - Compress the block
// 	  - Put the block in queue for sending
