#include <string.h>
#include "disk.h"
#include "file.h"

#define DISK_BLOCKS  10752
#define BLOCK_SIZE   2048
#define DATA_BLOCKS  10240

superBlock *super;
fatTable table[DATA_BLOCKS];
directory DirectoryTable[DATA_BLOCKS];
int valueAssigned = 0;

int intialize(){
	char temp[15];
	strcpy(temp,"virtual_disk");
	close_disk();

	make_disk(temp);

	superBlock super;
	super.startFatTable = 1;
	super.startDirectoryBlock = 17;
	super.startDataBlock = 512;
	super.numberOfFiles = 0;

	fatTable table[DATA_BLOCKS];
	for(int i=0;i<DATA_BLOCKS;i++){
		table[i].blockContent = -2;
	}	

	open_disk(temp);
	block_write(0,(char *)&super);
	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(fatTable)); ++i)
	{
		block_write(super.startFatTable+i,(char *)(table+i*(BLOCK_SIZE/sizeof(fatTable))));
	}
	close_disk();
	return 1;
}

int assigneTables(){
	char buf[2050];
	block_read(0,buf);
	super = (superBlock *) buf;
	
	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(fatTable)); ++i){
		block_read(super->startFatTable +i, buf);
		table + i*(BLOCK_SIZE/sizeof(fatTable)) = (fatTable *)buf;
	}

	if(super->numberOfFiles != 0){
		for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(directory)); ++i){
			block_read(super->startDirectoryBlock +i, buf);
			DirectoryTable + i*(BLOCK_SIZE/sizeof(directory)) = (directory *)buf;
			if(super->numberOfFiles <= i*(BLOCK_SIZE/sizeof(directory)))
				break;
		}
	}
	
	valueAssigned = 1;
}

int saveTables(){
	block_write(0,(char *)super);
	
	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(fatTable)); ++i)
	{
		block_write(super->startFatTable+i,(char *)(table+i*(BLOCK_SIZE/sizeof(fatTable))));
	}

	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(directory)); ++i){
		block_read(super->startDirectoryBlock +i, (char *)(DirectoryTable + i*(BLOCK_SIZE/sizeof(directory))));
		if(super->numberOfFiles <= i*(BLOCK_SIZE/sizeof(fatTable)))
			break;
	}
}

int fCreate(char *name){			//returns 1 or 0
	char temp[15];
	strcpy(temp,"virtual_disk");
	close_disk();
	if(isDiskCreated(temp)==-1)
		intialize();

	open_disk(temp);
	if(valueAssigned==0)
		assigneTables();

	int currentIndex = 0;
	while(currentIndex<DATA_BLOCKS && table[currentIndex].blockContent!=-2){
		currentIndex++;
	}
	if(currentIndex==DATA_BLOCKS)
		return 0;
	
	table[currentIndex].blockContent = -1;

	strcpy(DirectoryTable[super->numberOfFiles].fileName,name);
	DirectoryTable[super->numberOfFiles].startBlock = currentIndex;
	DirectoryTable[super->numberOfFiles].numberBlocks = 1;
	DirectoryTable[super->numberOfFiles].valid = 1;
	super->numberOfFiles++;
	return 1;
}

int fRemove(char *name){			//returns 1 or 0
	char temp[15];
	strcpy(temp,"virtual_disk");
	close_disk();
	if(isDiskCreated(temp)==-1)
		intialize();

	open_disk(temp);
	if(valueAssigned==0)
		assigneTables();

	for(int i=0;i<super->numberOfFiles;i++){
		if(strcasecmp(name,DirectoryTable[super->numberOfFiles + i].fileName)==0){
			DirectoryTable[super->startFatTable + i].valid = 0;
			int startDataBlock = DirectoryTable[super->startFatTable + i].startBlock;
			while(startDataBlock!=-1){
				int temp = table[startDataBlock];
				table[startDataBlock] = -2;
				startDataBlock = temp;
			}

		}
	}
}

int fOpen(char *name, char* permissions);			//returns -1 or unique fileDescriptor
int fClose(char *name);			//returns -1 or unique fileDescriptor


int fRead(int fDescriptor, char *buf, int size);			//returns 1 or 0
int fWrite(int fDescriptor, char *buf, int size);			//returns 1 or 0

int fRename(char *oldName, char *newName);			//returns 1 or 0
int fList(char **list);					//return -1 or number of files