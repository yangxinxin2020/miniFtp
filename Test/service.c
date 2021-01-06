//socket
#include<sys/types.h>
#include<sys/socket.h>
//htonl
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>

//close
#include<unistd.h>
#define PORT 3339

int main()
{
    char *sendbuf = "Thanks For Inviting!!!";
    char buf[256];
    int ser_fd,cli_fd;
    int ser_len,cli_len;
    struct sockaddr_in ser_addr;
    struct sockaddr_in cli_addr;
    ser_fd = socket(AF_INET,SOCK_STREAM,0);
    
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr= htonl(INADDR_ANY);
    ser_addr.sin_port = PORT;
    ser_len = sizeof(ser_addr);
    bind(ser_fd,(struct sockaddr *)&ser_addr,ser_len);
    listen(ser_fd,10);
    while(1)
    {
        printf("Please wait a moment!\n");
	cli_len = sizeof(cli_addr);
cli_fd = accept(ser_fd,(struct sockaddr *)&cli_addr,(socklen_t * __restrict)&cli_len);
	recv(cli_fd,buf,256,0);
	buf[sizeof(buf)+1] = '\0';
	printf("Receive message:\n %s\n",buf);
	send(cli_fd,sendbuf,sizeof(sendbuf),0);
	close(cli_fd);
    }
}
