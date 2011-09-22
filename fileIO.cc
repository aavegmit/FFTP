#include "fileIO.h"


unsigned char *fileMap;  /* mmapped array of int's */
struct stat fileStat;
list<uint64_t > sequenceNumberList;

//function to load file into global variable for MMAP...'fileMap'
void loadFileToMMap(){

	int fd;
	//File name from Client through TCP
        fd = open(FILE_NAME, O_RDONLY);
	if (fd == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}
			        
	if (fstat (fd, &fileStat) == -1) {
		perror ("fstat");
		exit(EXIT_FAILURE);
	}
	
	fileMap = (unsigned char *)mmap(0, fileStat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	//fileSize = sb.st_size;
	
	if (fileMap == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
}

//finally unloads the file from fileMap
void unloadFileMap(){

	if (munmap(fileMap, fileStat.st_size) == -1) {
		perror("Error un-mmapping the file");
		exit(EXIT_FAILURE);
	}
}

//populates the sequence number for file
void populateSequenceNumberList(){

	uint64_t q = fileStat.st_size/MAXDATASIZE;
	uint64_t r = fileStat.st_size%MAXDATASIZE;

	for(uint64_t i=0;i<q;i++){
		sequenceNumberList.push_back(i);
	}
	if(r > 0){
		sequenceNumberList.push_back(q);
	}
/*	while(!sequenceNumberList.empty()){
		printf("sequence number is : %d\n",sequenceNumberList.front());
		sequenceNumberList.pop_front();
	}
*/
}

void printMMapToFile(){

FILE *f = fopen("temp", "wb");
	while(fileStat.st_size > 0){
		fwrite(&fileMap[fileStat.st_size],1,1,f);
		fileStat.st_size--;
	}
fclose(f);
}

void getDataFromFile(uint64_t sequenceNum, unsigned char blockData[], uint64_t *size){

long int i = 0;
	if(fileStat.st_size - (off_t)sequenceNum*MAXDATASIZE < MAXDATASIZE){
		for(i = 0;i<(int)(fileStat.st_size - (off_t)sequenceNum*MAXDATASIZE);i++){
			blockData[i] = fileMap[(off_t)(sequenceNum*MAXDATASIZE+i)];
			*size =(uint64_t)( fileStat.st_size - (off_t)sequenceNum*MAXDATASIZE);
		}
	}
	else{
		for(i = 0;i<MAXDATASIZE;i++){
			blockData[i] = fileMap[(off_t)(sequenceNum*MAXDATASIZE+i)];
			*size = MAXDATASIZE;
		}
	}
}
