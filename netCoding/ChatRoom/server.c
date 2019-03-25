#include "csapp.h"
void echo(int connfd);
void command(void);
struct cNode{
    int clientFd;
};
struct cNode clients[1024];
int lens=0;
int setnonblocking(int sockfd){
	fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0)|O_NONBLOCK);
	return 0;
}
int main(int argc,char * * argv){
    for(int i=0;i<1024;i++){
        clients[i].clientFd=-1;
    }
    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    fd_set read_set,ready_set;
    if(argc!=2){
        fprintf(stderr,"Usage: %s <port>\n",argv[0]);
        exit(0);
    }
    int port=atoi(argv[1]);
    listenfd=Open_listenfd(port);
    printf("start to listen: \n");
    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO,&read_set);
    FD_SET(listenfd,&read_set);
    setnonblocking(listenfd);
    lens=listenfd;

    while(1){
        ready_set=read_set;
        printf("start select: \n");
        Select(lens+1,&ready_set,NULL,NULL,NULL);
        printf("after select\n");
        if(FD_ISSET(STDIN_FILENO,&ready_set)){
            command();
        }
        if(FD_ISSET(listenfd,&ready_set)){
            clientlen=sizeof(struct sockaddr_storage);
            connfd=Accept(listenfd,(SA *)&clientaddr,&clientlen);
            clients[lens].clientFd=connfd;
            if(lens<connfd){
                lens=connfd;
            }
            FD_SET(connfd,&read_set);
            setnonblocking(connfd);
            printf("%d add to FD_SET \n clients lens:%d\n",connfd,lens);
            //echo(connfd);
            //Close(connfd);
        }else{
            printf("recevied message from clients: ");
            clientlen=sizeof(struct sockaddr_storage);
            for(int i=0;i<lens;i++){
                if(FD_ISSET(clients[i].clientFd,&ready_set)){
                    printf("%d\n",clients[i].clientFd);
                    char recvBuf[4096];
                    int recvSize=recv(clients[i].clientFd,recvBuf,4096,0);
                    if(recvSize!=-1){
                        for(int ii=0;ii<lens;ii++){
                            if(clients[ii].clientFd!=-1&&ii!=i){
                                char message[4096];
                                sprintf(message,"client%d: %s\n",clients[ii].clientFd,recvBuf);
                                printf("message: %s\n",message);
                                send(clients[ii].clientFd,message,4096,0);
                            }
                        }
                    }
                }
            }
            //printf("recevied from %d\n",connfd);
        }
    }
}

void command(void){
    char buf[MAXLINE];
    if(!Fgets(buf,MAXLINE,stdin))
        exit(0);
    printf("%s",buf);
}

void echo(int connfd){
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio,connfd);
    while((n=Rio_readlineb(&rio,buf,MAXLINE))){
        printf("server received %d bytes\n",(int)n);
        for(int i=0;i<lens;i++){
            //if(clients[i].clientFd!=connfd)
                Rio_writen(clients[i].clientFd,buf,n);
        }
    }
}