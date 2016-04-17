#ifndef _FILE_H_
#define _FILE_H_

#define DISK_BLOCKS  10752
#define BLOCK_SIZE   2048

struct superBlock{
	unsigned short int startFatTable;
	unsigned short int startDirectoryBlock;
	unsigned short int startDataBlock;
	unsigned short int numberOfFiles;
};

struct fatTable{
	short int blockContent;
	// empty block = -2
	// end of file = -1
	// else index of next block
};

struct directory{
	char fileName[22];
	unsigned short int startBlock;
	int numberBlocks;
	int valid;
};

struct fileDescriptor{ //range from 0 to 31
	char fName[20];
	char mode[2];			// r or w or rw
	unsigned short int currentBlock;
	char *ptr;
	char *currentptr;
	int valid;
};

int intialize();
int assignTables();
int saveTables();

int fCreate(char *name);			//returns 1 or 0
int fRemove(char *name);			//returns 1 or 0

int fOpen(char *name, char* permissions);			//returns -1 or unique fileDescriptor
int fClose(int descriptor);			//returns -1 or unique fileDescriptor

int fRead(int fDescriptor, char *buf, int size);			//returns 1 or 0
int fWrite(int fDescriptor, char *buf, int size);			//returns 1 or 0

int fRename(char *oldName, char *newName);			//returns 1 or 0
int fList();					//return -1 or number of files

#endif
