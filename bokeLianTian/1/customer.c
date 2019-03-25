#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h> //for gethostbyname
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUF 4096
#define SERVER_PORT 12138


int main(int argc,char *argv[])
{
    int sockfd;//socket
    char sendBuf[MAX_BUF],recvBuf[MAX_BUF];
    int sendSize,recvSize;//用于记录记录发送和接收到数据的大小
    struct hostent * host;
    struct sockaddr_in servAddr;
    char username[32];
    char * p;

    if(argc != 3)
    {
        perror("use: ./client [hostname] [username]");
        exit(-1);
    }
    p=username;
    strcpy(p,argv[2]);
    printf("username:%s\n",username);
    host=gethostbyname(argv[1]);
    if(host==NULL)
    {
        perror("fail to get host by name.");
        exit(-1);
    }
    printf("Success to get host by name ...\n");

    //创建socket
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("fail to establish a socket");
        exit(1);
    }
    printf("Success to establish a socket...\n");
    
    /*init sockaddr_in*/
    servAddr.sin_family=AF_INET;
    servAddr.sin_port=htons(SERVER_PORT);
    servAddr.sin_addr=*((struct in_addr *)host->h_addr_list[0]);
    bzero(&(servAddr.sin_zero),8);

    /*connect the socket*/
    if(connect(sockfd,(struct sockaddr *)&servAddr,sizeof(struct sockaddr_in))==-1)
    {
        perror("fail to connect the socket");
        exit(1);
    }
    printf("Success to connect the socket...\n");

    //send-recv
    while(1)
    {
        printf("Input:");
        fgets(sendBuf,MAX_BUF,stdin);
        send(sockfd,sendBuf,strlen(sendBuf),0);
        memset(sendBuf,0,sizeof(sendBuf));
        recv(sockfd,recvBuf,MAX_BUF,0);
        printf("Server:%s\n",recvBuf);
        memset(recvBuf,0,sizeof(recvBuf));
    }

    return 0;
}