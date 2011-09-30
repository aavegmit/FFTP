#include "fileIO.h"
#include "shared.h"


unsigned char *fileMap;  /* mmapped array of int's */
struct stat fileStat;
list<uint64_t > sequenceNumberList[NUM_UDP_CONNECTION];
pthread_mutex_t sequenceNumberListLock[NUM_UDP_CONNECTION];
pthread_cond_t sequenceNumberListCV[NUM_UDP_CONNECTION];
unsigned char *mapToFile;
struct fileInfo fileInfoObj;
param objParam;
bool lastPacketReceived ;
unsigned char *bitV;
map<long, bool> toBeSend;
long packetsRcvd;



//function to load file into global variable for MMAP...'fileMap'
void loadFileToMMap(){

    int fd;
    //File name from Client through TCP
    fd = open(fileInfoObj.fileName.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    if (fstat (fd, &fileStat) == -1) {
        perror ("fstat");
        exit(EXIT_FAILURE);
    }

    fileMap = (unsigned char *)mmap(0, fileInfoObj.fileStat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    //fileSize = sb.st_size;

    if (fileMap == MAP_FAILED) {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
}

//finally unloads the file from fileMap
void unloadFileMap(){

    if (munmap(fileMap, fileInfoObj.fileStat.st_size) == -1) {
	perror("Error un-mmapping the file");
	exit(EXIT_FAILURE);
    }
}

//populates the sequence number for file
/*void populateSequenceNumberList(){

    pthread_mutex_lock(&sequenceNumberListLock);
    uint64_t q = fileInfoObj.fileStat.st_size/MAXDATASIZE;
    uint64_t r = fileInfoObj.fileStat.st_size%MAXDATASIZE;

    objParam.noOfSeq = q ;
    for(uint64_t i=0;i<q;i++){
        sequenceNumberList.push_back(i);
	toBeSend[i] = true ;
    }
    if(r > 0){
        sequenceNumberList.push_back(q);
	++objParam.noOfSeq ;
	toBeSend[q] = true ;
    }
    pthread_mutex_unlock(&sequenceNumberListLock);
}*/

void pushSequenceNumberInList(uint64_t sequenceNum){
    static int myTurn = 0;
    if(myTurn == NUM_UDP_CONNECTION){
        myTurn = 0;
    }

    pthread_mutex_lock(&sequenceNumberListLock[myTurn]);
    sequenceNumberList[myTurn].push_front(sequenceNum);
    pthread_cond_signal(&sequenceNumberListCV[myTurn]);
    myTurn++;
    pthread_mutex_unlock(&sequenceNumberListLock[myTurn-1]);

}
void printMMapToFile(){

    FILE *f = fopen("temp", "wb");
    while(fileStat.st_size > 0){
        fwrite(&fileMap[fileStat.st_size],1,1,f);
        fileStat.st_size--;
    }
    fclose(f);
}

void getDataFromFile(uint64_t sequenceNum, unsigned char blockData[], uint32_t *size){

    long int i = 0;
    if(fileStat.st_size - (off_t)sequenceNum*MAXDATASIZE < MAXDATASIZE){
        for(i = 0;i<(int)(fileStat.st_size - (off_t)sequenceNum*MAXDATASIZE);i++){
	    blockData[i] = fileMap[(off_t)(sequenceNum*MAXDATASIZE+i)];
	}
	*size =(uint32_t)( fileStat.st_size - (off_t)sequenceNum*MAXDATASIZE);
    }
    else{
	for(i = 0;i<MAXDATASIZE;i++){
	    blockData[i] = fileMap[(off_t)(sequenceNum*MAXDATASIZE+i)];
	}
	*size = MAXDATASIZE;
    }
}

//Write Thread of UDP server
void *WriteToFileThread(void *args){

    int count1 = 0 ;
    udpMessage mes;
    int fd = loadMMapForFile((unsigned char *)objParam.localFilePath.c_str());
    memset(mes.buffer, '\0', MAXDATASIZE);

    while(1){
	pthread_mutex_lock(&udpMessageClientQLock);
	if(udpMessageClientQ.empty()){
	    ++count1 ;
            pthread_cond_wait(&udpMessageClientQCV, &udpMessageClientQLock);
        }

        mes = udpMessageClientQ.front();
        udpMessageClientQ.pop_front();
        pthread_mutex_unlock(&udpMessageClientQLock);

        for(int i = 0; i < mes.data_len; i++){
            mapToFile[mes.sequenceNum*MAXDATASIZE+i] = mes.buffer[i];
        }
	// Check for the bitvector- termination
	if(mes.sequenceNum == objParam.noOfSeq - 1){
	    lastPacketReceived = true ;
	    printf("Last packet received....\n") ;
	}
	if (lastPacketReceived)
	    if (udpMessageClientQ.empty())
		if (isBitVectorSet(bitV))
		    break ;

	if(udpMessageClientQ.empty() && packetsRcvd == objParam.noOfSeq){
	    printf("All packet received....\n") ;
	    break ;
	}
	
        memset(mes.buffer, '\0', MAXDATASIZE);
    }
    printf("WriteToFile thread exiting at client...\n");
    unloadMMapForFile(fd);
    printf("Duplicate Packets count %d\n", dropPacketCount) ;
    printf("Write to file thread wait count %d\n", count1) ;
    shutDown();
    exit(0) ;
}

// creating a mmap for writing to a file at the end
int loadMMapForFile(unsigned char fileName[]){

    int fd;
    int result;
    //unsigned char *map;  /* mmapped array of int's */

    /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */
    fd = open((char *)fileName, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    result = lseek(fd, objParam.fileSize -1, SEEK_SET);
    if (result == -1) {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
     * have the file actually have the new size.
     * Just writing an empty string at the current file position will do.
     *
     * Note:
     *  - The current position in the file is at the end of the stretched 
     *    file due to the call to lseek().
     *  - An empty string is actually a single '\0' character, so a zero-byte
     *    will be written at the last byte of the file.
     */
    result = write(fd, "", 1);
    if (result != 1) {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.
     */
    mapToFile = (unsigned char *)mmap(0, objParam.fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapToFile == MAP_FAILED) {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    return fd;
}


void unloadMMapForFile(int fd) {

    /* Don't forget to free the mmapped memory
     */
    if (munmap(mapToFile, objParam.fileSize) == -1) {
        perror("Error un-mmapping the file");
        /* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);
}
