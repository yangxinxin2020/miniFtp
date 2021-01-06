//send
#include<sys/types.h>
#include<sys/socket.h>

#include<stdio.h>
//htonl

#include<netinet/in.h>
#include<arpa/inet.h>
//close
#include<unistd.h>
#define PORT 3339
int main()
{
 
/*
int socket(int domain, int type, int protocol);
参数domin 代表所用的协议族，通常为AF_INET，表示互联网协议族，type 的可取值为SOCK_STREAM或者SOCK_DGRAM(数据包套接字).SOCK_RAW(原始套接字)，protocol 通常为 0;
通用的套接字数据结构的定义如下:
struct sockaddr
{
	unsigned short sa_family; 地址族AF_***
	char sa_data[14]; 14字节的协议地址  包含ip 地址和端口号
}

IPV4套接字数据结构
struct sockaddr_in
{
	short int sin_family; 地址族
	unsigned short int sin_port;端口号
	struct in_addr sin_addr;IP地址
	unsigned char sin_zero[0]; 填充0以保持与struct sockaddr 同样大小
}
struct in_addr 结构体：
struct in_addr {

    in_addr_t s_addr;表示一个32位的IPv4地址。

};
打印的时候可以调用inet_ntoa()函数将其转换为char*类型。

头文件为：#include <arpa/inet.h>

inet——ntoa()函数用于将一个十进制网络字节序转换为点分十进制IP格式的字符串。

函数原型为：char*inet_ntoa(struct in_addr in);

头文件为：arpa/inet.h
*/
    char *sendbuf = "Thanks!";
    char buf[256];
    int s_fd,c_fd;//服务器和客户端套接字标识符
    int s_len,c_len;//服务器和客户端消息长度
/********************************************************/
    struct sockaddr_in s_addr; //服务器套接字地址
    struct sockaddr_in c_addr; //客户端套接字地址
//	IPV4套接字数据结构
/********************************************************/

    s_fd = socket(AF_INET,SOCK_STREAM,0);//创建套接字
/*********************************************************
IPV4套接字数据结构
struct sockaddr_in
{	short int sin_family; 地址族
	unsigned short int sin_port;端口号
	struct in_addr sin_addr;IP地址
	unsigned char sin_zero[0]; 填充0以保持与struct sockaddr 同样大小
}
struct in_addr 结构体：
struct in_addr {
    in_addr_t s_addr;表示一个32位的IPv4地址。
};
*********************************************************/

    s_addr.sin_family = AF_INET;//定义服务器套接字地址中的域
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);//定义套接字地址
	s_addr.sin_port = PORT;   //定义服务器套接字端口
	s_len = sizeof(s_addr);
/*
int bind(int sockfd,const struct sockaddr *my_addr,socklen_t addrlen);
sockfd 是调用socket()函数返回的套接字描述符,参数my_addr是一个指向包含有本机IP地址
以及端口号等信息的sockaddr类型的指针,参数addrlen 通常被用来设置为结构体struct sockaddr的长度
在使用bind函数时，需要将sin_port 转换为网络字节优先顺序,而sin_addr则不需要转换
my_addr.sin_addr.s_add=INADDR_ANY 填入本机的IP地址
*/
	bind(s_fd,(struct sockaddr *)&s_addr,s_len);//绑定套接字
/*
int  listen(int sockfd,int backlog);
sockfd是socket()函数返回的套接字描述,backlog指定在请求队列中允许的最大请求数目，进入的链接请求将在队列中等待accept()等系统的操作。
*/
	listen(s_fd,10); //监听状态
    while(1){
		printf("please wait a moment!\n");
		c_len = sizeof(c_addr);
		//接收客户端的连接请求
/*
在建立输入队列后，服务器就调用accept()函数,然后睡眠并等待客户端的连接请求
int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
参数sockfd 是sosket()函数返回的套接字描述符，addr通常是一个指向sockaddr_in变量的指针，该变量用来存放提出链接提出服务的主机的信息；
addrlen 通常为一个指向值为sizeof(struct sockaddr_in)的整形指针变量。
*/
        c_fd = accept(s_fd,(struct sockaddr *)&c_addr,(socklen_t * __restrict)&c_len);
		//接收消息
/*
ssize_t recv(int s,void *buf,size_t len,int flags);
s为socket()函数返回的套接字的描述符，参数buf是存档接收数据的缓存区，参数len是缓存区的长度，flags是0
*/
		recv(c_fd,buf,256,0);
		buf[sizeof(buf)+1]='\0';
		printf("receive message:\n %s\n",buf); //输出到终端
/*
ssize_t send(int sockfd,sonst void *msg,size_t len,int falgs);
sockfd 是socket()函数返回的套接字描述符，参数msg是一个要发送数据的指针；参数len是以字节为单位的数据的长度，flags一般情况为0；
*/
        send(c_fd,sendbuf,sizeof(sendbuf),0);//回复消息
/*
int close(int fd);
参数fd是调用socket()函数时返回的套接字描述符.
*/
        close(c_fd);
	}
}
