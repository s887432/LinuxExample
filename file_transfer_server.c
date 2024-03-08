#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define DEBUG_TIME

#ifdef DEBUG_TIME
#include <sys/time.h>
#include <unistd.h>
#endif

#define SIZE 1024

void write_file(int sockfd)
{
    int n; 
    FILE *fp;
    char *filename = "file2.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    if(fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }
    while(1)
    {
        n = recv(sockfd, buffer, SIZE, 0);
        if(n<=0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
    
}

int main ()
{
    int port = 1812;
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("[-]Error in socket");
        exit(1);
    }
     printf("[+]Server socket created. \n\r");

     server_addr.sin_family = AF_INET;
     server_addr.sin_port = port;
     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

     e = bind(sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr));
     if(e<0)
     {
         perror("[-]Error in Binding");
         exit(1);
     }
     printf("[+]Binding Successfull.\n\r");

do {
     e = listen(sockfd, 10);
     if(e==0)
     {
         printf("[+]Listening...\n\r");
     }
     else 
     {
         perror("[-]Error in Binding");
         exit(1);
     }
     addr_size = sizeof(new_addr);
     new_sock = accept(sockfd,(struct sockaddr*)&new_addr, &addr_size);

#ifdef DEBUG_TIME
	struct timeval start, end;
	gettimeofday(&start, NULL);
#endif

     write_file(new_sock);
     
#ifdef DEBUG_TIME
    gettimeofday(&end, NULL);
    long long time_es = ((long long)end.tv_sec * 1000000 + end.tv_usec) - 
				((long long)start.tv_sec * 1000000 + start.tv_usec);

	printf("\033[3;31m%lld uSec\033[;0m\n\r", time_es);
#endif

     printf("[+]Data written in the text file \n\r");
}while(1);

	close(new_sock);
	close(sockfd);
}
