#include<sys/types.h>
#include<sys/socket.h>

#include<stdio.h>

#include<netinet/in.h>
#include<arpa/inet.h>

#include<unistd.h>

#define PORT 3333

int main()
{

  int sockfd,newsockfd;
  int lensock,nelen;
  char *sendbuf = "Com On!";
  char buf[500];
  struct sockaddr_in addr;
  sockfd = socket(AF_INET,SOCK_STREAM,0);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = PORT;
  lensock = sizeof(addr);
  newsockfd = connect(sockfd,(struct sockaddr *)&addr,lensock);
  if(newsockfd == -1)
  {
     perror("\n 连接错误！\n");
     return 1;
  }  
  recv(sockfd,buf,256,0);
  buf[sizeof(buf)+1] = '\0';
  printf("\n%s\n",buf);
  send(sockfd,sendbuf,sizeof(sendbuf),0);
  sleep(10);
  recv(sockfd,buf,256,0);
  buf[sizeof(buf)+1] = '\0';
  printf("\n Rece from ser info is : %s \n",buf);
  close(sockfd);

  return 0;
}
