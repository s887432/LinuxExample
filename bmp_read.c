#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

typedef struct tagBITMAPFILEHEADER
{
    unsigned short bfType;  //specifies the file type
    unsigned int bfSize;  //specifies the size in bytes of the bitmap file
    unsigned short bfReserved1;  //reserved; must be 0
    unsigned short bfReserved2;  //reserved; must be 0
    unsigned int bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    unsigned int biSize;  //specifies the number of bytes required by the struct
    int biWidth;  //specifies width in pixels
    int biHeight;  //species height in pixels
    unsigned short biPlanes; //specifies the number of color planes, must be 1
    unsigned short biBitCount; //specifies the number of bit per pixel
    unsigned int biCompression;//spcifies the type of compression
    unsigned int biSizeImage;  //size of image in bytes
    int biXPelsPerMeter;  //number of pixels per meter in x axis
    int biYPelsPerMeter;  //number of pixels per meter in y axis
    unsigned int biClrUsed;  //number of colors used by th ebitmap
    unsigned int biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

void showFileHeader(BITMAPFILEHEADER *bitmapFileHeader)
{
	printf("%X\n\r", bitmapFileHeader->bfType);
	printf("%X\n\r", bitmapFileHeader->bfSize);
	printf("%X %X\n\r", bitmapFileHeader->bfReserved1, bitmapFileHeader->bfReserved2);
	printf("%X\n\r", bitmapFileHeader->bfOffBits);
}

void showBmpHeader(BITMAPINFOHEADER *bitmapInfoHeader)
{
	printf("%X\n\r", bitmapInfoHeader->biSize);
	printf("%X\n\r", bitmapInfoHeader->biWidth);
	printf("%X\n\r", bitmapInfoHeader->biHeight);
	printf("%X\n\r", bitmapInfoHeader->biPlanes);
	printf("%X\n\r", bitmapInfoHeader->biBitCount);
	printf("%X\n\r", bitmapInfoHeader->biCompression);
	printf("%X\n\r", bitmapInfoHeader->biSizeImage);
	printf("%X\n\r", bitmapInfoHeader->biXPelsPerMeter);
	printf("%X\n\r", bitmapInfoHeader->biYPelsPerMeter);
	printf("%X\n\r", bitmapInfoHeader->biClrUsed);
	printf("%X\n\r", bitmapInfoHeader->biClrImportant);
}

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
	fread(&bitmapFileHeader.bfType, sizeof(unsigned short), 1, filePtr);
	fread(&bitmapFileHeader.bfSize, sizeof(unsigned int), 1, filePtr);
	fread(&bitmapFileHeader.bfReserved1, sizeof(unsigned short), 1, filePtr);
	fread(&bitmapFileHeader.bfReserved2, sizeof(unsigned short), 1, filePtr);
	fread(&bitmapFileHeader.bfOffBits, sizeof(unsigned int), 1, filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

	//showFileHeader(&bitmapFileHeader);

    //read the bitmap info header
	fread(&bitmapInfoHeader->biSize, sizeof(unsigned int), 1, filePtr);
	fread(&bitmapInfoHeader->biWidth, sizeof(int), 1, filePtr);
	fread(&bitmapInfoHeader->biHeight, sizeof(int), 1, filePtr);
	fread(&bitmapInfoHeader->biPlanes, sizeof(unsigned short), 1, filePtr);
	fread(&bitmapInfoHeader->biBitCount, sizeof(unsigned short), 1, filePtr);
	fread(&bitmapInfoHeader->biCompression, sizeof(unsigned int), 1, filePtr);
	fread(&bitmapInfoHeader->biSizeImage, sizeof(unsigned int), 1, filePtr);
	fread(&bitmapInfoHeader->biXPelsPerMeter, sizeof(int), 1, filePtr);
	fread(&bitmapInfoHeader->biYPelsPerMeter, sizeof(int), 1, filePtr);
	fread(&bitmapInfoHeader->biClrUsed, sizeof(unsigned int), 1, filePtr);
	fread(&bitmapInfoHeader->biClrImportant, sizeof(unsigned int), 1, filePtr);

	//showBmpHeader(bitmapInfoHeader);

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	bitmapInfoHeader->biSizeImage = bitmapInfoHeader->biWidth*bitmapInfoHeader->biHeight*(bitmapInfoHeader->biBitCount>>3);
    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader->biSizeImage, 1, filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    ////swap the r and b values to get RGB (bitmap is BGR)
    //for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3) // fixed semicolon
    //{
    //    tempRGB = bitmapImage[imageIdx];
    //    bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
    //    bitmapImage[imageIdx + 2] = tempRGB;
    //}

    //close file and return bitmap iamge data
    fclose(filePtr);

	for(imageIdx=0; imageIdx<10; imageIdx++)
	{
			printf("%02X ", bitmapImage[imageIdx]);
	}

	printf("\r\n");

    return bitmapImage;
}

int main(int argc, char *argv[])
{
	unsigned char *bmp;
	BITMAPINFOHEADER bitmapInfoHeader;

	if( argc != 2 )
	{
		printf("Usage:\n");
		printf("\tbmp_reader bitmap_file\n");
		exit(5);
	}

	bmp = LoadBitmapFile(argv[1], &bitmapInfoHeader);

	if( NULL == bmp )
	{
		printf("Can't load : %s\n\r", argv[1]);
	}
	else
	{
		free(bmp);
	}

	return 0;
}

// end of file

