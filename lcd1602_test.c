#include <stdio.h>  			// printf
#include <stdlib.h>				// strtoul
#include <fcntl.h>  			// open, O_RDWD
#include <string.h>				// strlen
#include <unistd.h>				// write, Sleep, close
#include <sys/ioctl.h>			// ioctl
#include <linux/i2c-dev.h>  	// I2C_SLAVE
#include <errno.h>				// errno

#define SEND_COMMAND	0x04	// send command: EN=1, RW=0, RS=0
#define SEND_DATA		0x05	// send data: EN=1, RW=0, RS=1
#define SEND_MASK		0xFB	// disable EN: EN=0

int BLEN = 1;

void lcd_WriteByte(int fd, unsigned char data)
{
	unsigned char temp = data;

	if( BLEN == 1 )
	{
		temp |= 0x08;
	}
	else
	{
		temp &= 0xF7;
	}

	write(fd, &temp, 1);	
}

// data[7:4]: 4 bits data
// data[3]	: N/A
// data[2]	: EN
// data[1]	: RW
// data[0]	: RS
void lcd_Send(int fd, int comm, unsigned char flag)
{
	int temp;

	// send bit[7:4]
	temp = comm & 0xF0;
	temp |= flag;
	lcd_WriteByte(fd, temp);
	usleep(2000);
	temp &= SEND_MASK;
	lcd_WriteByte(fd, temp);

	// send bit[3:0]
	temp = ((comm & 0x0F) << 4);
	temp |= flag;
	lcd_WriteByte(fd, temp);
	usleep(2000);
	temp &= SEND_MASK;
	lcd_WriteByte(fd, temp);
}

void lcd_Init(int fd)
{
	lcd_Send(fd, 0x33, SEND_COMMAND);
	usleep(5000);
	lcd_Send(fd, 0x32, SEND_COMMAND);
	usleep(5000);
	lcd_Send(fd, 0x28, SEND_COMMAND);
	usleep(5000);
	lcd_Send(fd, 0x0C, SEND_COMMAND);
	usleep(5000);
	lcd_Send(fd, 0x01, SEND_COMMAND);
	
	lcd_WriteByte(fd, 0x08);
}

void lcd_WriteString(int fd, int x, int y, char *data)
{
	int addr, i;
	int tmp;

	if( x < 0 ) x = 0;
	if( x > 15 ) x = 15;
	if( y < 0 ) y = 0;
	if( y > 1 ) y = 1;

	addr = 0x80 + 0x40 * y + x;
	lcd_Send(fd, addr, SEND_COMMAND);

	tmp = strlen(data);
	for(i=0; i<tmp; i++)
	{
		lcd_Send(fd, data[i], SEND_DATA);
	}
}

void lcd_Clear(int fd)
{
	lcd_Send(fd, 0x01, SEND_COMMAND);
}

int main (int argc, char *argv[])
{
	unsigned char slave_addr;
	char **ptr;
	int fd;

	if( argc != 3 )
	{
		printf("Usage: lcd1602_test DEVICE_NODE SLAVE_ADDR(in HEX)\n\r");
		printf("Ex: ./lcd1602_test /dev/i2c-2 4c\n\r");
		return 0;
	}

	slave_addr = (unsigned char)strtoul(argv[2], ptr, 16);

	fd = open(argv[1], O_RDWR);
	if( fd < 0 )
	{
		printf("Open dvice fail. (%s)\n\r", argv[2]);
		return 0;
	}

	if (ioctl(fd, I2C_SLAVE, slave_addr) < 0)
	{
		printf("ioctl error: %s\n", strerror(errno));  
		return 1;  
	}

	lcd_Init(fd);

	lcd_WriteString(fd, 0, 0, "Hello World!!!");
	lcd_WriteString(fd, 0, 1, "Patrick Lin");

	close(fd);
	return 0;  
}

// end of file


