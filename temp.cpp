#include "file.h"
#include <iostream>
#include <string.h>
using namespace std;

int main(){
	char temp[15];
	intialize();
	strcpy(temp,"temp1");
	cout<<fCreate(temp)<<"--\n";

	char per[2];
	strcpy(per,"w");

	int i = fOpen(temp,per);	
	char temp1[100];
	strcpy(temp1,"AERAHWEFJASDJFKASDFVKWEAFJJASDJFAJWEFASDJFAWEGMFASDKFAWEGNJASDFMASJGAKEG");
	for(int qwe=0;qwe<30;qwe++){
		fWrite(i,temp1,sizeof(temp1));
	}
	fClose(i);

	strcpy(per,"r");
	i = fOpen(temp,per);
	char temp2[100];
	for(int qwe=0;qwe<30;qwe++){
		int qa = fRead(i,temp2,sizeof(temp1));
		cout<<temp2<<"\n";
	}
	fClose(i);
	
	fList();
	saveTables();
	return 0;
}