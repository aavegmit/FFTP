#include <iostream>
#include <cstdio>

using namespace std ;


void *TCPserverThread(void *);

void *TCPreadThread(void *);

void *TCPwriteThread(void *);

void *UDPserverThread(void *);

void *UDPreadThread(void *);

void *UDPwriteThread(void *);
