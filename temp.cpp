#include <string.h>
#include "disk.h"

int main(){
	char temp[100];
	strcpy(temp,"virtual_disk");
	make_disk(temp);
	return 0;
}