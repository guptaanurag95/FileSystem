#include "file.h"
#include <iostream>
#include <string.h>
using namespace std;

int main(){
	char temp[15];
	// intialize();
	strcpy(temp,"tasdemp1asdf");
	fCreate(temp);
	strcpy(temp,"tempsadf2sdf");
	fCreate(temp);
	
	// char list[100][30];
	// char *c_list = list[0];
	fList();
	fRemove(temp);
	cout<<"============\n";
	fList();
	saveTables();
	// for(int i=0;i<len;i++)
		// cout<<i<<" "<<list[i];
	return 0;
}