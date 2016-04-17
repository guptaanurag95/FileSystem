#include "file.h"
#include <iostream>
#include <string.h>
using namespace std;

int main(){
	char temp[15];
	intialize();
	strcpy(temp,"temp1");
	fCreate(temp);

	char per[2];
	strcpy(per,"w");

	int i = fOpen(temp,per);
	cout<<i<<"\n";
	char temp1[20];
	strcpy(temp1,"tasdfasdfjkhsfemp1");
	fWrite(i,temp1,sizeof(temp1));
	cout<<"Closeing the file : "<<fClose(i)<<"\n";

	// strcpy(per,"r");
	// i = fOpen(temp,per);
	// char temp2[20];
	// fRead(i,temp2,sizeof(temp1));
	// cout<<temp2<<"===\n";
	// fClose(i);
	
	fList();
	saveTables();
	return 0;
}