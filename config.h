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

#define TCP_PORT "4710"
#define MAXDATASIZE 2000
#define TCP_BACKLOG 10
#define TCP_HEADER_SIZE 5
#define NUM_UDP_CONNECTION 10 
#define RANDOM_PACKET 0
#define LOST_PACKET 1
#define MAX_CACHE_SIZE 5
#define ACK_SENDING_RATIO 20 
#define CRITICAL_ACK_SENDING_GAP 2000
#define ACK_RESPONSE_DELAY 10
#define LOSS_RATIO 0
#define LOSS_LIMIT 40
#define SLOW_SENDER 20
#define MAX_UDP_Q_SIZE 200
