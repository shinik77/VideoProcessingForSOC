#include <stdio.h>
typedef unsigned short	U16;

void save_binaries(U16* buf)
{
	//file write using U16* buf pointer
	//save and open binaries. compare with bmp pixel values
	FILE *fp;
	if ((fp = fopen("samplebin", "wt")) == NULL)
	{
		printf("file open failed");
		return;
	}
	fwrite((void *)buf, 1, sizeof(buf), fp);
	fclose(fp);
	printf("file is written.");
}


int main(void){
	U16* integer;
	*integer = 23244;
	save_binaries(integer);
	return 0;
}
