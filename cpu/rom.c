#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen("../psilver.gbc", "rb");
	fseek(fileptr, 0, SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);

	buffer = (char *)malloc((filelen+1)*sizeof(char)); 
	fread(buffer, filelen, 1, fileptr);
	fclose(fileptr); 
	
	printf("%lu", ((uint8_t *)buffer)[0]);
}