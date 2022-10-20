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

char *frameBuffer = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

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

void showFileHeader(BITMAPFILEHEADER *pbitmapFileHeader)
{
	printf("Type: %04X\r\n", pbitmapFileHeader->bfType);
	printf("File size=%d\r\n", pbitmapFileHeader->bfSize);
	printf("Bitmap offset=%d\r\n", pbitmapFileHeader->bfOffBits);
}

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
	printf("Loading file: %s\r\n", filename);
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

	fseek(filePtr, 0, SEEK_SET);
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

    //close file and return bitmap iamge data
    fclose(filePtr);

    return bitmapImage;
}

#if 0
void draw2Framebuffer(BITMAPINFOHEADER *bitmapInfoHeader, unsigned char *bitmapData, unsigned char *fbp)
{
	const int fb_bytesPerPixel = 2; 
	const int stride = finfo.line_length / fb_bytesPerPixel;
	unsigned char *dest = (unsigned char *) (frameBuffer) + (vinfo.yoffset) * stride + (vinfo.xoffset);

	int height = bitmapInfoHeader->biHeight;
	int width = bitmapInfoHeader->biWidth;
	int bpp = bitmapInfoHeader->biBitCount>>3;
	int x, y;
	unsigned char *source = bitmapData;

	int src_index = 0, des_index = 0;

	int iii = 0;

	printf("Width = %d\r\n", width);
	printf("Height = %d\r\n", height);
	printf("Bpp = %d\r\n", bpp);

	for (y = 0; y < height; ++y) 
	{ 
		des_index = (height - y - 1) * fb_bytesPerPixel * width;
		
		for (x = 0; x < width; x++) 
		{
			if ( bpp == 3) { 
				unsigned int red = bitmapData[src_index+2]>>3; 
				unsigned int green = bitmapData[src_index+1]>>2; 
				unsigned int blue = bitmapData[src_index]>>3; 
				unsigned short color16 = red << 11 | green << 5 | blue;

				dest[des_index] = color16&0xFF;
				dest[des_index+1] = color16>>8;

				des_index += fb_bytesPerPixel;
				src_index += bpp;
			}
		} 
	}
}
#endif

int main(int argc, char *argv[])
{
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	char *fbp;
	int x=0, y=0;
	long int location = 0;

	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char *bitmapData;

	fbfd = open("/dev/fb0", O_RDWR);
	if( !fbfd )
	{
		printf("Can't open framebuffer devie\n\r");
		return 1;
	}

	if( ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) != 0)
	{
		printf("Can't read fixed information\n\r");
		close(fbfd);
		return 1;
	}

	if( ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) != 0)
	{
		printf("Can't read cariable information\n\r");
		close(fbfd);
		return 1;
	}

	printf("Display information:\r\n");
	printf("\tWidth = %d\r\n", vinfo.xres);
	printf("\tHeight = %d\r\n", vinfo.yres);
	printf("\tBpp = %d\r\n", vinfo.bits_per_pixel);

	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	fbp = (char*)mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

	bitmapData = LoadBitmapFile(argv[2],&bitmapInfoHeader);

	if( NULL == bitmapData )
	{
		printf("Load bitmap file error\n\r");
		return 0;
	}

	printf("Image info\r\n");
	printf("\tWidth = %d\r\n", bitmapInfoHeader.biHeight);
	printf("\tHeight = %d\r\n", bitmapInfoHeader.biWidth);
	printf("\tBpp = %d\r\n", bitmapInfoHeader.biBitCount>>3);

	int src_index = 0;
	for(y=0; y<vinfo.yres; y++)
		{
			for(x=vinfo.xres-1; x>=0; x--)
			{
				location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset)*finfo.line_length;
				if( vinfo.bits_per_pixel == 16 )
				{
					unsigned int red = bitmapData[src_index+2]>>3; 
					unsigned int green = bitmapData[src_index+1]>>2; 
					unsigned int blue = bitmapData[src_index]>>3; 
					unsigned short color = ((red&0x1F) << 11 | (green&0x3F) << 5 | (blue&0x1F)) & 0xFFFF;

					*(fbp+location) = (color >> 8) & 0xFF;
					*(fbp+location + 1) = color & 0xFF;
				}
				src_index+=(bitmapInfoHeader.biBitCount>>3);
			}
		}

	free(bitmapData);

	munmap(fbp, screensize);
	close(fbfd);

	return 0;
}

// end of file
