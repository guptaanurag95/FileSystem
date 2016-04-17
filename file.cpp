#include <string.h>
#include "disk.h"
#include "file.h"

#define DISK_BLOCKS  10752
#define BLOCK_SIZE   2048
#define DATA_BLOCKS  10240
#define NUMBER_FAT__TABLE_BLOCKS (sizeof(fatTable)*DATA_BLOCKS)/BLOCK_SIZE

int intialize(){
	char temp[15];
	strcpy(temp,"virtual_disk");
	close_disk(temp);

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
	block_write(super.startFatTable,(char *)table);
	return 1;
}

int fCreate(char *name){			//returns 1 or 0
	char temp[15];
	strcpy(temp,"virtual_disk");
	close_disk(temp);
	if(isDiskCreated(temp)==-1)
		intialize();

	char buf[2050];

	block_read(0,buf);
	superBlock *super;
	super = (superBlock *) buf;

	block_read(super->startFatTable,buf);
	fatTable *table;
	table = (fatTable *) buf;
	int currentIndex = 1;
	while(currentIndex<=DATA_BLOCKS && table->blockContent!=-2){
		currentIndex++;
		table++;
	}
	if(currentIndex==DATA_BLOCKS+1)
		return -1;

	int blockNumber = super->startFatTable + (currentIndex*sizeof(fatTable))/BLOCK_SIZE - 1;
	if((currentIndex%(BLOCK_SIZE/sizeof(fatTable)))!=0)
		blockNumber++;

	block_read(blockNumber,buf);
	fatTable *table1;
	table1 = (fatTable *)buf;
	if((currentIndex%(BLOCK_SIZE/sizeof(fatTable)))==0)
		table1 = table1 + ((BLOCK_SIZE/sizeof(fatTable))-2);
	else table1 = table1 + (currentIndex%(BLOCK_SIZE/sizeof(fatTable))) - 1;
	table1->blockContent = -1;
	block_write(blockNumber,buf);

	block_read(super->startDirectoryBlock,buf);
	super->numberOfFiles
}

int fRemove(char *name);			//returns 1 or 0

int fOpen(char *name, char* permissions);			//returns -1 or unique fileDescriptor
int fClose(char *name);			//returns -1 or unique fileDescriptor


int fRead(int fDescriptor, char *buf, int size);			//returns 1 or 0
int fWrite(int fDescriptor, char *buf, int size);			//returns 1 or 0

int fRename(char *oldName, char *newName);			//returns 1 or 0
int fList(char **list);					//return -1 or number of files