/*
* FILE: ntp.c
* NOTE: socket网络编程学习，NTP时间获取程序
*
* TIME: 2021年11月13日00:05:39
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>

#define   NTP_PORT   123
#define   TIME_PORT  37
#define   NTP_SERVER_IP  "time1.facebook.com"

#define   NTP_PORT_STR   "123"
#define   NTPV1       "NTP/V1"
#define NTPV2 "NTP/V2"

#define NTPV3 "NTP/V3"
#define NTPV4 "NTP/V4"
#define TIME "TIME/UDP"

#define   NTP_PCK_LEN   48

#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4
#define PREC -6

#define JAN_1970 0x83aa7e80 /* 1900 年～1970 年之间的时间秒数 */
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x)) >> 11))
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

typedef struct _ntp_time
{
    unsigned int coarse;
    unsigned int fine;
} ntp_time;

/* NTP时钟同步报文 */
struct ntp_packet
{
    unsigned char leap_ver_mode;
    unsigned char startum;
    char poll;
    char precision;
    int root_delay;
    int root_dispersion;
    int reference_identifier;
    ntp_time reference_timestamp;
    ntp_time originage_timestamp;
    ntp_time receive_timestamp;
    ntp_time transmit_timestamp;
};

char protocol[32];

int construct_packet(char *packet)
{
    char version = 1;
    long tmp_wrd;
    int port;
    time_t timer;
    strcpy(protocol, NTPV4);
    /*判断协议版本*/
    if(!strcmp(protocol, NTPV1)||!strcmp(protocol, NTPV2)||!strcmp(protocol, NTPV3)||!strcmp(protocol, NTPV4))
    {
        memset(packet, 0, NTP_PCK_LEN);
        port = NTP_PORT;
        /*设置 16 字节的包头*/
        version = protocol[5] - 0x30;
        tmp_wrd = htonl((LI << 30)|(version << 27) \
            |(MODE << 24)|(STRATUM << 16)|(POLL << 8)|(PREC & 0xff));
        memcpy(packet, &tmp_wrd, sizeof(tmp_wrd));
        
        /*设置 Root Delay、 Root Dispersion 和 Reference Indentifier */
        tmp_wrd = htonl(1<<16);
        memcpy(&packet[4], &tmp_wrd, sizeof(tmp_wrd));
        memcpy(&packet[8], &tmp_wrd, sizeof(tmp_wrd));
        /*设置 Timestamp 部分*/
        time(&timer);
        /*设置 Transmit Timestamp coarse*/
        tmp_wrd = htonl(JAN_1970 + (long)timer);
        memcpy(&packet[40], &tmp_wrd, sizeof(tmp_wrd));
        /*设置 Transmit Timestamp fine*/
        tmp_wrd = htonl((long)NTPFRAC(timer));
        memcpy(&packet[44], &tmp_wrd, sizeof(tmp_wrd));
        return NTP_PCK_LEN;
    }
    else if (!strcmp(protocol, TIME))/* "TIME/UDP" */
    {
        port = TIME_PORT;
        memset(packet, 0, 4);
        return 4;
    }

    return 0;
}

/*获取 NTP 时间*/
int get_ntp_time(int sk, struct addrinfo *addr, struct ntp_packet *ret_time)
{
    fd_set pending_data;
    struct timeval block_time;
    char data[NTP_PCK_LEN * 8];
    int packet_len, data_len = addr->ai_addrlen, count = 0, result, i,re;
    printf("start get_ntp_time １\n");
    /* 组织请求报文 */
    if (!(packet_len = construct_packet(data)))
    {
        return 0;
    }
    /*客户端给服务器端发送 NTP 协议数据包*/
	printf("start get_ntp_time 2 \n");
    if ((result = sendto(sk, data, packet_len, 0, addr->ai_addr, data_len)) < 0)
    {
        perror("sendto");
        return 0;
    }
    /*调用select()函数，并设定超时时间为10s*/
    FD_ZERO(&pending_data);
    FD_SET(sk, &pending_data);
    block_time.tv_sec=10;
    block_time.tv_usec=0;
	printf("start get_ntp_time 3\n");
    if (select(sk + 1, &pending_data, NULL, NULL, &block_time) > 0)
    {
        /*接收服务器端的信息*/
		printf("start get_ntp_time 4\n");
        if ((count = recvfrom(sk, data, NTP_PCK_LEN * 8, 0, addr->ai_addr, &data_len)) < 0)
        {
            perror("recvfrom");
            return 0;
        }
		printf("start get_ntp_time 5\n");
        if (protocol == TIME)
        {
            memcpy(&ret_time->transmit_timestamp, data, 4);
            return 1;
        }
        else if (count < NTP_PCK_LEN)
        {
            return 0;
        }
        printf("start get_ntp_time 6\n");
        /* 设置接收 NTP 包的数据结构 */
        ret_time->leap_ver_mode = ntohl(data[0]);
        ret_time->startum = ntohl(data[1]);
        ret_time->poll = ntohl(data[2]);
        ret_time->precision = ntohl(data[3]);
        ret_time->root_delay = ntohl(*(int*)&(data[4]));
        ret_time->root_dispersion = ntohl(*(int*)&(data[8]));
        ret_time->reference_identifier = ntohl(*(int*)&(data[12]));
        ret_time->reference_timestamp.coarse = ntohl(*(int*)&(data[16]));
        ret_time->reference_timestamp.fine = ntohl(*(int*)&(data[20]));
        ret_time->originage_timestamp.coarse = ntohl(*(int*)&(data[24]));
        ret_time->originage_timestamp.fine = ntohl(*(int*)&(data[28]));
        ret_time->receive_timestamp.coarse = ntohl(*(int*)&(data[32]));
        ret_time->receive_timestamp.fine = ntohl(*(int*)&(data[36]));
        ret_time->transmit_timestamp.coarse = ntohl(*(int*)&(data[40]));
        ret_time->transmit_timestamp.fine = ntohl(*(int*)&(data[44]));

        /* 将NTP时间戳转换为日期 */
        time_t currentTime = ret_time->transmit_timestamp.coarse - JAN_1970;
        struct tm CurlocalTime;
        localtime_r(&currentTime, &CurlocalTime);
        char dateTime[30];
        strftime(dateTime, 30, "%Y-%m-%d %H:%M:%S %A", &CurlocalTime);

        printf("%s\n", dateTime);
    
        return 1;
    } /* end of if select */
   
    
    return 0;
}

/* 修改本地时间 */
int set_local_time(struct ntp_packet * pnew_time_packet)
{
    struct timeval tv;
    tv.tv_sec = pnew_time_packet->transmit_timestamp.coarse - JAN_1970;
    tv.tv_usec = USEC(pnew_time_packet->transmit_timestamp.fine);
    return settimeofday(&tv, NULL);
}

int main(int argc, char *argv[])
{
    int sockfd, rc;
    struct addrinfo hints, *res = NULL;
    struct ntp_packet new_time_packet;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    
    /*调用 getaddrinfo()函数， 获取地址信息*/
    rc = getaddrinfo(NTP_SERVER_IP, NTP_PORT_STR, &hints, &res);
    if (rc != 0)
    {
        perror("getaddrinfo");
        return 1;
    }

    /* 创建套接字 */
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol); //IPv4, 数据报套接字, UDP
    if (sockfd <0 )
    {
        perror("socket");
        return 1;
    }
	printf("start get_ntp_time \n");
    /*调用取得 NTP 时间的函数*/
    if (get_ntp_time(sockfd, res, &new_time_packet))
    {
        /*调整本地时间*/
        //if (!set_local_time(&new_time_packet))
        {
            printf("NTP client success!\n");
        }
    }
    
    close(sockfd);

    return 0;
}