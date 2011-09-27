/* 
 * Group - 3-Lost-Packets
 * config.h
 *
 * This file includes all the macros used in the protocol
 * The values will be tweaked in order to get the best
 * possible result.
 *
 * This file is shared among the server and the client
 *
 */

// Maximum size of the MAP (virtual memory concept as suggested by TEO)

#define TCP_PORT "78471"
#define MAXDATASIZE 1450
#define TCP_BACKLOG 10
#define TCP_HEADER_SIZE 5
#define NUM_UDP_CONNECTION 10
#define CLIENT_NAME "localhost"
#define FILE_NAME "/media/My DATA/PICS/WALLPAPERS/google_search.jpg"
//#define FILE_NAME "TODO"
#define RANDOM_PACKET 0
#define LOST_PACKET 1
#define MAX_CACHE_SIZE 100
#define ACK_SENDING_RATIO 5
