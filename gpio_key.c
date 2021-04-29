#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int keys_fd;
	struct input_event t;

	if( (keys_fd = open("/dev/input/event0", O_RDONLY)) <= 0 )
	{
		printf("open event error\r\n");
		return -1;
	}

	while(1)
	{
		if( read(keys_fd, &t, sizeof(t)) == sizeof(t))
		{
			if( t.type == EV_KEY )
			{
				if( t.value ==0 || t.value == 1 )
				{
					printf("key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
					if( t.code == KEY_ESC )
					{
						break;
					}
				}
			}
		}
	}

	close(keys_fd);
	return 0;
}

// end of file

