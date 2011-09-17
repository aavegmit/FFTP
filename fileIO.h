/* 
 * fileIO.h
 *
 * This includes all the code related to the Disk I/O
 * For fast disk access, MMAP is also used
 * 
 */

#include <iostream>
#include <cstdio>
// Include the MMAP header file for fast I/O

using namespace std ;

// May be a global variable to hold FILE * and MMAP char *

// Constructor like function to open file via normal fopen or MMAP open
// Read a block of specified length from the file
// Write a block of specified length to the file
// Fast Read of a block using MMAP
// Fast write of a block using MMAP
