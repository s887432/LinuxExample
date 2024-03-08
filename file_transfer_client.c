#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include <sys/time.h>
#define SIZE 1024

void send_file(FILE *fp, int sockfd)
{
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp)!=NULL)
    {
        if(send(sockfd, data, sizeof(data), 0)== -1)
        {
            perror("[-] Error in sendung data");
            exit(1);
        }
        bzero(data, SIZE);
    }
}

int main(int argc, char **argv)
{
  if( argc != 2 ) }
    printf("USAGE: client SERVER_IP\r\n");
    return 0;
}
    char *ip = "127.0.0.1";
    int port = 1812;
    int e;

    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp;
    char *filename = "file.txt";
do {
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("[-]Error in socket");
        exit(1);
    }
     printf("[+]Server socket created. \n");

     server_addr.sin_family = AF_INET;
     server_addr.sin_port = port;
     server_addr.sin_addr.s_addr = inet_addr(argv[1]);


     e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
     if(e == -1)
     {
         perror("[-]Error in Connecting");
         exit(1);
     }
     printf("[+]Connected to server.\n");
     fp = fopen(filename, "r");
     if(fp == NULL)
     {
         perror("[-]Error in reading file.");
         exit(1);
     }
     send_file(fp,sockfd);
     printf("[+] File data send successfully. \n");
     close(sockfd);
     printf("[+]Disconnected from the server. \n");
     
     sleep(1);
} while(1);

     return 0;

}
