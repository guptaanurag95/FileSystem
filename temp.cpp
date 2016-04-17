#include "file.h"
#include <iostream>
#include <string.h>
using namespace std;

int main(){
	char temp[15];
	intialize();
	strcpy(temp,"temp1");
	fCreate(temp);
	strcpy(temp,"temp2");
	fCreate(temp);
	
	// char list[100][30];
	// char *c_list = list[0];
	fList();
	char temp1[15];
	strcpy(temp1,"temp1qwe");
	fRename(temp,temp1);
	cout<<"============\n";
	fList();
	saveTables();
	// for(int i=0;i<len;i++)
		// cout<<i<<" "<<list[i];
	return 0;
}