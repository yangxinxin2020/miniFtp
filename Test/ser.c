#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>

#define PORT 3333

int main()
{
   int s_fd,c_fd;
   int s_len,c_len;
   char *sendbuf = "Thanks!";
   char *sendst = "Hi!cli!";
   struct sockaddr_in s_addr;
   struct sockaddr_in c_addr;
   char buf[256];
   s_fd = socket(AF_INET,SOCK_STREAM,0);
   s_addr.sin_family = AF_INET;
   s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   s_addr.sin_port = PORT;
   s_len = sizeof(s_addr);
   bind(s_fd,(struct sockaddr *)&s_addr,s_len);
   listen(s_fd,20);
   while(1)
   {
       printf("\nPlease Wait Moment!\n");
       c_len = sizeof(c_addr);
       c_fd = accept(s_fd,(struct sockaddr *)&c_addr,(socklen_t *__restrict )&c_len);
       send(c_fd,sendst,sizeof(sendst),0);
       sleep(10);
       recv(c_fd,buf,256,0);
       buf[sizeof(buf)+1] = '\0';
       printf("\nRece message:\n%s",buf);
       send(c_fd,sendbuf,sizeof(sendbuf),0);
       close(c_fd);
   }
}
