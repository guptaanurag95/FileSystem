#include <string.h>
#include "disk.h"
#include "file.h"
#include <iostream>
using namespace std;

#define DISK_BLOCKS  10752
#define BLOCK_SIZE   2048
#define DATA_BLOCKS  10240
#define TOTAL_FILE_DESCRIPTOR 32

superBlock super;
fatTable table[DATA_BLOCKS];
directory DirectoryTable[DATA_BLOCKS];
fileDescriptor Descriptor[33];
int numberofOpenFiles = 0;
int valueAssigned = 0;

int intialize(){
	char temp[15];
	strcpy(temp,"virtual_disk");

	make_disk(temp);

	superBlock super;
	super.startFatTable = 1;
	super.startDirectoryBlock = 100;
	super.startDataBlock = 512;
	super.numberOfFiles = 0;

	fatTable table[DATA_BLOCKS];
	for(int i=0;i<DATA_BLOCKS;i++){
		table[i].blockContent = -2;

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
	}
	close_disk();
	return 1;
}


int assignTables(){
	for(int j=0;j<TOTAL_FILE_DESCRIPTOR;j++){
		Descriptor[j].valid = 0;
	}

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
		temp = table + i*(BLOCK_SIZE/sizeof(fatTable));
		memcpy(temp, buf, BLOCK_SIZE);
	}

	if(super.numberOfFiles != 0){
		for (int i = 0; i < DATA_BLOCKS/(BLOCK_SIZE/sizeof(directory)); ++i){
			
			memset(buf, 0, BLOCK_SIZE);
			block_read(super.startDirectoryBlock +i, buf);
			directory *temp1;
			temp1=DirectoryTable + i*(BLOCK_SIZE/sizeof(directory));
			memcpy(temp1, buf, BLOCK_SIZE);
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
	if(isDiskCreated(temp)==-1)
		intialize();
	close_disk();

	open_disk(temp);
	if(valueAssigned==0)
		assignTables();
	
	int i;
	for(i=0;i<super.numberOfFiles;i++)
		if(strcasecmp(name,DirectoryTable[i].fileName)==0 && DirectoryTable[i].valid==1)
		{
			break;
		}
	if(i<super.numberOfFiles){
		saveTables();
		return 0;
	}

	int currentIndex = 0;
	while(currentIndex<DATA_BLOCKS && table[currentIndex].blockContent!=-2){

		currentIndex++;
	}
	if(currentIndex==DATA_BLOCKS){
		saveTables();
		return 0;
	}
	
	table[currentIndex].blockContent = -1;

	strcpy(DirectoryTable[super.numberOfFiles].fileName,name);
	DirectoryTable[super.numberOfFiles].startBlock = currentIndex + super.startDataBlock;
	DirectoryTable[super.numberOfFiles].numberBlocks = 1;
	DirectoryTable[super.numberOfFiles].valid = 1;
	super.numberOfFiles++;
	saveTables();
	return 1;
}

int fRemove(char *name){			//returns 1 or 0
	char temp[15];
	strcpy(temp,"virtual_disk");
	if(isDiskCreated(temp)==-1)
		intialize();
	close_disk();

	open_disk(temp);
	if(valueAssigned==0)
		assignTables();

	for(int i=0;i<super.numberOfFiles;i++){
		if(strcasecmp(name,DirectoryTable[i].fileName)==0 && DirectoryTable[i].valid==1){
			DirectoryTable[i].valid = 0;
			int startDataBlock = DirectoryTable[i].startBlock - super.startDataBlock;
			while(table[startDataBlock].blockContent!=-1){
				int temp = table[startDataBlock].blockContent;
				table[startDataBlock].blockContent = -2;
				startDataBlock = temp;
			}
			saveTables();
			return 1;
			break;
		}
	}
	saveTables();
	return 0;
}

int fOpen(char *name, char* permissions){			//returns -1 or unique fileDescriptor
	char temp[15];
	strcpy(temp,"virtual_disk");
	if(isDiskCreated(temp)==-1)
		intialize();
	close_disk();

	open_disk(temp);
	if(valueAssigned==0)
		assignTables();

	int i,j=0;
	for(i=0;i<super.numberOfFiles;i++)
		if(strcasecmp(name,DirectoryTable[i].fileName)==0)
		{
			for(j=0;j<TOTAL_FILE_DESCRIPTOR;j++)
			{
				if(Descriptor[j].valid==0)
				{

						strcpy(Descriptor[j].fName,name);
						strcpy(Descriptor[j].mode,permissions);
						Descriptor[j].currentBlock = DirectoryTable[i].startBlock;

						block_read(DirectoryTable[i].startBlock,Descriptor[j].ptr);
						Descriptor[j].currentptr = Descriptor[j].ptr;
						Descriptor[j].valid = 1;
						break;	

				}
			}
			break;
		}
	saveTables();
	if(j==TOTAL_FILE_DESCRIPTOR || i==super.numberOfFiles)
		return -1;

	return j;
}

int fClose(int descriptor){			//returns -1 or 1
	if(Descriptor[descriptor].valid == 1){
		block_write(Descriptor[descriptor].currentBlock, Descriptor[descriptor].ptr);
		Descriptor[descriptor].valid = 0;
		saveTables();
		return 1;
	}
	saveTables();
	return -1;
}

int fRead(int descriptor, char *buf, int size){			//returns 1 or 0
	if(Descriptor[descriptor].valid != 1 || !(Descriptor[descriptor].mode[0]=='r' || Descriptor[descriptor].mode[1]=='r')){
		saveTables();
		return 0;
	}
	
	int sizeLeft = BLOCK_SIZE - (Descriptor[descriptor].currentptr - Descriptor[descriptor].ptr);
	if(size<=sizeLeft){
		int i;
		for(i=0;i<=size;i++){
			buf[i] = Descriptor[descriptor].currentptr[i];
		}

		Descriptor[descriptor].currentptr = Descriptor[descriptor].currentptr + size;
		saveTables();
		return 1;
	}
	else{
		int i;
		for(i=0;i<sizeLeft;i++){
			buf[i] = Descriptor[descriptor].currentptr[i];
		}

		int temp = Descriptor[descriptor].currentBlock - super.startDataBlock;
		if(table[temp].blockContent==-1){
			Descriptor[descriptor].currentptr = Descriptor[descriptor].currentptr + sizeLeft;
			saveTables();
			return 1;
		}
		Descriptor[descriptor].currentBlock = table[temp].blockContent + super.startDataBlock;

		block_read(Descriptor[descriptor].currentBlock,Descriptor[descriptor].ptr);
		Descriptor[descriptor].currentptr = Descriptor[descriptor].ptr;
		for(i=sizeLeft;i<=size;i++){
			buf[i] = Descriptor[descriptor].currentptr[i-sizeLeft];
		}
		Descriptor[descriptor].currentptr = Descriptor[descriptor].currentptr + (size - sizeLeft);
		saveTables();
		return 1;
	}
}

int fWrite(int descriptor, char *buf, int size){			//returns 1 or 0
	if(Descriptor[descriptor].valid != 1 || !(Descriptor[descriptor].mode[0]=='w' || Descriptor[descriptor].mode[1]=='w')){
		saveTables();
		return 0;
	}
	int sizeLeft = BLOCK_SIZE - (Descriptor[descriptor].currentptr - Descriptor[descriptor].ptr);

	if(size<=sizeLeft){
		strcpy(Descriptor[descriptor].currentptr,buf);
		Descriptor[descriptor].currentptr = Descriptor[descriptor].currentptr + size;
		saveTables();
		return 1;
	}
	else{
		for(int i=0;i<sizeLeft;i++){
			Descriptor[descriptor].currentptr[i] = buf[i];
		}

		block_write(Descriptor[descriptor].currentBlock, Descriptor[descriptor].ptr);

		int temp = Descriptor[descriptor].currentBlock - super.startDataBlock;

		if(table[temp].blockContent==-1){
			int currentIndex = 0;
			while(currentIndex<DATA_BLOCKS && table[currentIndex].blockContent!=-2){
				currentIndex++;
			}
			if(currentIndex==DATA_BLOCKS){
				saveTables();
				return 0;
			}

			table[temp].blockContent = currentIndex;
			table[currentIndex].blockContent = -1;
			Descriptor[descriptor].currentBlock = temp + super.startDataBlock;	
		}
		Descriptor[descriptor].currentBlock = table[temp].blockContent + super.startDataBlock;	
		
		block_read(Descriptor[descriptor].currentBlock,Descriptor[descriptor].ptr);
		Descriptor[descriptor].currentptr = Descriptor[descriptor].ptr;
		for(int i=sizeLeft;i<=size;i++){
			Descriptor[descriptor].currentptr[i-sizeLeft] = buf[i];
		}
		Descriptor[descriptor].currentptr = Descriptor[descriptor].currentptr + (size - sizeLeft);
		saveTables();
		return 1;
	}
}

int fRename(char *oldName, char *newName)		//returns 1 or 0
{
	char temp[15];
	strcpy(temp,"virtual_disk");
	if(isDiskCreated(temp)==-1)
		intialize();
	close_disk();

	open_disk(temp);
	if(valueAssigned==0)
		assignTables();

	int i;
	for(i=0;i<super.numberOfFiles;i++)
		if(strcasecmp(newName,DirectoryTable[i].fileName)==0 && DirectoryTable[i].valid==1)
		{
			break;
		}
	if(i<super.numberOfFiles){
		saveTables();
		return 0;
	}

	for(i=0;i<super.numberOfFiles;i++)
		if(strcasecmp(oldName,DirectoryTable[i].fileName)==0 && DirectoryTable[i].valid==1){
			strcpy(DirectoryTable[i].fileName,newName);
			break;
		}
	saveTables();
	if(i<super.numberOfFiles)
		return 1;
	if(i>=super.numberOfFiles)
		return 0;
}

int fList()				//return -1 or number of files
{
	char temp[15];
	strcpy(temp,"virtual_disk");
	if(isDiskCreated(temp)==-1)
		intialize();
	close_disk();

	open_disk(temp);
	if(valueAssigned==0)
		assignTables();

	int list_index = 0, i;
	for(i=0;i<super.numberOfFiles;i++){
		if(DirectoryTable[i].valid == 1){
			cout<<DirectoryTable[i].fileName<<"\n";
			list_index++;
		}
	}
	saveTables();
	if(list_index==0)
		return -1;
	else
		return list_index;
}
