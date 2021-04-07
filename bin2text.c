#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *fpInput, *fpOutput;

	if( (fpInput=fopen(argv[1], "rb")) == NULL )
	{
		printf("Can't open source file: \"%s\"\r\n", argv[1]);
		return -1;
	}

	if( (fpOutput=fopen(argv[2], "wb")) == NULL )
	{
		printf("Can't open output file: \"%s\"\r\n", argv[2]);
		fclose(fpInput);
		return -1;
	}

	int length = 0;
	int fileLength = 0;

	fseek(fpInput, 0L, SEEK_END);
	fileLength = ftell(fpInput);
	printf("Source size = %d\r\n", fileLength);
	fseek(fpInput, 0L, SEEK_SET);

	unsigned char *buffer;
	buffer = (unsigned char*)malloc(fileLength);
	fread(buffer, fileLength, 1, fpInput);
	fclose(fpInput);

	int start = 0;
	int skip = 0;
	for(int i=0; i<16; i++)
	{
		printf("0x%02X", buffer[i]);
		if( i < 15 )
			printf(", ");

		if( buffer[i] >= 0x20 && skip == 0 && i > 4)
		{
			start = i;
			skip = 1;
		}
	}

	printf("\n");
	printf("Start from offset = %d\n", start);

	for(int i=start; i<fileLength; i++)
	{
		if( buffer[i] == 0 && buffer[i+1] == 0)
		{
			break;
		}

		if( buffer[i] == 0 )
		{
			fprintf(fpOutput, "\n");
		}
		else
		{
			fprintf(fpOutput, "%c", buffer[i]);
		}
	}

	free(buffer);	
	fclose(fpOutput);

	return 0;
}


