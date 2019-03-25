#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_PORT 12138
#define BACKLOG 20
#define MAX_CON_NO 10
#define MAX_DATA_SIZE 4096

static void sig_usr1(int singno)
{
    exit(1);
}

int main(int argc,char *argv[])
{
    struct sockaddr_in serverSockaddr,clientSockaddr;
    char sendBuf[MAX_DATA_SIZE],recvBuf[MAX_DATA_SIZE];
    int sendSize,recvSize;
    int sockfd,clientfd;
    int on=1;
    socklen_t sinSize=0;
    char username[32];
    int pid;
    int Queue[MAX_CON_NO+1];
    int queue_ptr;
    int i;

    if(argc != 2)
    {
        printf("usage: ./server [username]\n");
        exit(1);
    }
    strcpy(username,argv[1]);
    printf("username:%s\n",username);

    /*establish a socket*/
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("fail to establish a socket");
        exit(1);
    }
    printf("Success to establish a socket...\n");

    /*init sockaddr_in*/
    serverSockaddr.sin_family=AF_INET;
    serverSockaddr.sin_port=htons(SERVER_PORT);
    serverSockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    bzero(&(serverSockaddr.sin_zero),8);

    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    /*bind socket*/
    if(bind(sockfd,(struct sockaddr *)&serverSockaddr,sizeof(struct sockaddr))==-1)
    {
        perror("fail to bind");
        exit(1);
    }
    printf("Success to bind the socket...\n");

    /*listen on the socket*/
    if(listen(sockfd,BACKLOG)==-1)
    {
        perror("fail to listen");
        exit(1);
    }

    sinSize=sizeof(clientSockaddr);//注意要写上，否则获取不了IP和端口
    queue_ptr=0;
    while(1)//多次accept
    {
        /*accept a client's request*/
        if((clientfd=accept(sockfd,(struct sockaddr  *)&clientSockaddr, &sinSize))==-1)
        {
            perror("fail to accept");
            exit(1);
        }
        printf("Success to accpet a connection request...\n");
        printf(">>>>>> %s:%d join in!\n",inet_ntoa(clientSockaddr.sin_addr),ntohs(clientSockaddr.sin_port));
        Queue[queue_ptr++]=clientfd;

        if(pid!=0)
        {
            kill(pid,SIGUSR1);
        }

        if((pid=fork())<0)
        {
            perror("fork error\n");
        }
        else if(pid==0)/*child*/
        {
            while(1)
            {
                /*receive datas from client*/
                if((recvSize=recv(clientfd,recvBuf,MAX_DATA_SIZE,0))==-1)
                {
                    perror("fail to receive datas");
                    exit(1);
                }
                printf("Client:%s\n",recvBuf);
                memset(recvBuf,0,MAX_DATA_SIZE);
            }
        }

        if((pid=fork())<0)
        {
            perror("fork error");
        }
        else if(pid==0)//child
        {
            /*send datas to client*/
            signal(SIGUSR1,sig_usr1);
            printf("现在有%d个人\n",queue_ptr);//没有考虑断开的问题
            while(1)
            {
                fgets(sendBuf,MAX_DATA_SIZE,stdin);
                for(i=0;i<queue_ptr;i++)
                {
                    if((sendSize=send(Queue[i],sendBuf,strlen(sendBuf),0))!=strlen(sendBuf))
                    {
                        perror("fail to send datas");
                        exit(1);
                    }
                    else
                    {
                        printf("Success to send datas\n");
                    }

                }
                memset(sendBuf,0,MAX_DATA_SIZE);
            }

        }
    }
    close(sockfd);

    return 0;
}