#include <string.h>
#include "disk.h"
#include "file.h"
#include <iostream>
using namespace std;

#define DISK_BLOCKS  10752
#define BLOCK_SIZE   2048
#define DATA_BLOCKS  10240

superBlock super;
fatTable table[DATA_BLOCKS];
directory DirectoryTable[DATA_BLOCKS];
fileDescriptor Descriptor[33];
int numberofOpenFiles = 0;
int valueAssigned = 0;

int intialize(){
	char temp[15];
	strcpy(temp,"virtual_disk");
	// close_disk();

	make_disk(temp);

	superBlock super;
	super.startFatTable = 1;
	super.startDirectoryBlock = 100;
	super.startDataBlock = 512;
	super.numberOfFiles = 0;

	fatTable table[DATA_BLOCKS];
	for(int i=0;i<DATA_BLOCKS;i++){
		table[i].blockContent = -2;
		// cout<<temp->blockContent<<"ASD\n";
	}

	char buffer[BLOCK_SIZE];
	memset(buffer, 0, BLOCK_SIZE);
	memcpy(buffer, &super, BLOCK_SIZE);

	open_disk(temp);
	block_write(0,buffer);

	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(fatTable)); ++i)
	{
		memset(buffer, 0, BLOCK_SIZE);
		memcpy(buffer, (table+i*(BLOCK_SIZE/sizeof(fatTable))), BLOCK_SIZE);
		block_write(super.startFatTable+i,buffer);
	}

	directory directoryTable[DATA_BLOCKS];
	for(int i=0;i<DATA_BLOCKS;i++){
		directoryTable[i].valid = -1;
		strcpy(directoryTable[i].fileName,"qwertyui");
		directoryTable[i].startBlock = -1;
		directoryTable[i].numberBlocks = -1;
	}

	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(directory)); ++i){
		memset(buffer, 0, BLOCK_SIZE);
		memcpy(buffer, (directoryTable+i*(BLOCK_SIZE/sizeof(directory))), BLOCK_SIZE);		
		block_write(super.startDirectoryBlock +i, buffer);
		// if(super.numberOfFiles <= i*(BLOCK_SIZE/sizeof(fatTable)))
		// 	break;
	}
	close_disk();
	return 1;
}


int assignTables(){
	char temp[15];
	strcpy(temp,"virtual_disk");
	open_disk(temp);

	char buf[BLOCK_SIZE];
	block_read(0,buf);
	memcpy(&super, buf, BLOCK_SIZE);
	
	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(fatTable)); ++i){
		memset(buf, 0, BLOCK_SIZE);
		block_read(super.startFatTable +i, buf);
		fatTable *temp;
		temp= table + i*(BLOCK_SIZE/sizeof(fatTable));
		memcpy(temp, buf, BLOCK_SIZE);
	}

	if(super.numberOfFiles != 0){
		for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(directory)); ++i){
			
			memset(buf, 0, BLOCK_SIZE);
			block_read(super.startDirectoryBlock +i, buf);
			directory *temp1;
			temp1=DirectoryTable + i*(BLOCK_SIZE/sizeof(directory));
			memcpy(temp1, buf, BLOCK_SIZE);
			// if(super.numberOfFiles <= i*(BLOCK_SIZE/sizeof(directory)))
			// 	break;
		}
	}
	
	valueAssigned = 1;
}


int saveTables(){
	char buf[BLOCK_SIZE];
	memset(buf, 0, BLOCK_SIZE);
	memcpy(buf, &super, BLOCK_SIZE);
	block_write(0,buf);
	
	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(fatTable)); ++i)
	{
		memset(buf, 0, BLOCK_SIZE);
		memcpy(buf, (table+i*(BLOCK_SIZE/sizeof(fatTable))), BLOCK_SIZE);
		block_write(super.startFatTable+i,buf);
	}

	for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(directory)); ++i){
		memset(buf, 0, BLOCK_SIZE);
		memcpy(buf, (DirectoryTable+i*(BLOCK_SIZE/sizeof(directory))), BLOCK_SIZE);		
		block_write(super.startDirectoryBlock +i, buf);
		// if(super.numberOfFiles <= i*(BLOCK_SIZE/sizeof(fatTable)))
		// 	break;
	}
}


int fCreate(char *name){			//returns 1 or 0
	char temp[15];
	strcpy(temp,"virtual_disk");
	close_disk();

	//-------- Inititalise-----------
	//if(isDiskCreated(temp)==-1)
//		intialize();

	open_disk(temp);
	if(valueAssigned==0)
		assignTables();
	// ------------------------------
	int currentIndex = 0;
	while(currentIndex<DATA_BLOCKS && table[currentIndex].blockContent!=-2){
		// cout<<table[currentIndex].blockContent<<"\n";
		currentIndex++;
	}
	if(currentIndex==DATA_BLOCKS)
		return 0;
	
	table[currentIndex].blockContent = -1;

	strcpy(DirectoryTable[super.numberOfFiles].fileName,name);
	DirectoryTable[super.numberOfFiles].startBlock = currentIndex;
	DirectoryTable[super.numberOfFiles].numberBlocks = 1;
	DirectoryTable[super.numberOfFiles].valid = 1;
	super.numberOfFiles++;
	return 1;
}

int fRemove(char *name){			//returns 1 or 0
	char temp[15];
	strcpy(temp,"virtual_disk");
	close_disk();
	//if(isDiskCreated(temp)==-1)
//		intialize();

	open_disk(temp);
	if(valueAssigned==0)
		assignTables();

	for(int i=0;i<super.numberOfFiles;i++){
		if(strcasecmp(name,DirectoryTable[i].fileName)==0){
			DirectoryTable[i].valid = -1;
			int startDataBlock = DirectoryTable[i].startBlock;
			while(startDataBlock!=-1){
				int temp = table[startDataBlock].blockContent;
				table[startDataBlock].blockContent = -2;
				startDataBlock = temp;
			}
			break;
		}
	}
}

int fOpen(char *name, char* permissions)			//returns -1 or unique fileDescriptor
{
	int i;
	for(i=0;i<super.numberOfFiles;i++)
		if(strcasecmp(name,DirectoryTable[i].fileName)==0)
		{
			strcpy(Descriptor[numberofOpenFiles+1].fName,name);
			strcpy(Descriptor[numberofOpenFiles+1].mode,permissions);
			//----Append-------
			// int blockContent = i
			// while(blockContent!=-2){
			// 	blockContent = fatTable[blockContent].blockContent
			// }
			// Descriptor[numberofOpenFiles+1].currentBlock = blockContent
			Descriptor[numberofOpenFiles+1].currentBlock = DirectoryTable[i].startBlock;

			block_read(DirectoryTable[i].startBlock,Descriptor[numberofOpenFiles+1].ptr);
			Descriptor[numberofOpenFiles+1].currentptr = Descriptor[numberofOpenFiles+1].ptr;
			numberofOpenFiles++;
			break;
		} 
	
}

int fClose(char *name);			//returns -1 or unique fileDescriptor

int fRead(int fDescriptor, char *buf, int size){			//returns 1 or 0

}

int fWrite(int fDescriptor, char *buf, int size);			//returns 1 or 0

int fRename(char *oldName, char *newName)		//returns 1 or 0
{
	int i;
	for(i=0;i<super.numberOfFiles;i++)
		if(strcasecmp(oldName,DirectoryTable[i].fileName)==0)
			strcpy(DirectoryTable[i].fileName,newName);  
	if(i<super.numberOfFiles)
		return 1;
	if(i>=super.numberOfFiles)
		return 0;
}

int fList()				//return -1 or number of files
{
	// cout<<"ASDF\n";
	int list_index = 0, i;
	// cout<<super.numberOfFiles<<"SADF\n";
	for(i=0;i<super.numberOfFiles;i++){
		// cout<<DirectoryTable[i].valid;
		if(DirectoryTable[i].valid == 1){
			// strcpy(list[list_index],DirectoryTable[i].fileName);
			cout<<DirectoryTable[i].fileName<<"\n";
			list_index++;
		}
	}
	if(list_index==0)
		return -1;
	else
		return list_index;
}
