#include <stdio.h>  			// printf
#include <stdlib.h>				// strtoul
#include <fcntl.h>  			// open, O_RDWD
#include <string.h>				// strlen
#include <unistd.h>				// write, Sleep, close
#include <sys/ioctl.h>			// ioctl
#include <linux/i2c-dev.h>  	// I2C_SLAVE
#include <errno.h>				// errno

#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

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

int get_ip(char *interface, char *ip_addr)
{
	int fd;
	struct ifreq ifr;

	if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
		return -1;
	}

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "eth0" */
	strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);

	if( ioctl(fd, SIOCGIFADDR, &ifr) < 0 )
	{
		printf("%s is not available\n", interface);
		return -1;
	}
	else
	{
		printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
		strcpy(ip_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	}

	close(fd);
	
	return 0;
}

int main (int argc, char *argv[])
{
	unsigned char slave_addr;
	char **ptr;
	int fd;
	char ip_addr[32];
	char tmp[32];

	if( argc != 3 && argc != 1)
	{
		printf("Usage: lcd1602_test DEVICE_NODE SLAVE_ADDR(in HEX)\n\r");
		printf("Ex: ./lcd1602_test /dev/i2c-2 4c\n\r");
		return 0;
	}

	if( argc == 3 )
	{
		slave_addr = (unsigned char)strtoul(argv[2], ptr, 16);
	}
	else
	{
		slave_addr = 0x27;
	}

	if( argc == 3 )
	{
		fd = open(argv[1], O_RDWR);
	}
	else
	{
		fd = open("/dev/i2c-2", O_RDWR);
	}

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

	//lcd_WriteString(fd, 0, 0, "Hello World!!!");
	//lcd_WriteString(fd, 0, 1, "Stupid Hitomi");
	if( get_ip("eth0", ip_addr) < 0 )
	{
		sprintf(tmp, "eth0: N/A");	
	}
	else
	{
		sprintf(tmp, "e%s", ip_addr);
	}

	lcd_WriteString(fd, 0, 0, tmp);

	if( get_ip("wlan0", ip_addr) < 0 )
	{
		sprintf(tmp, "wlan0: N/A");	
	}
	else
	{
		sprintf(tmp, "w%s", ip_addr);
	}

	lcd_WriteString(fd, 0, 1, tmp);


	close(fd);
	return 0;  
}

// end of file


