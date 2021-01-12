#include"sysutil.h"


int tcp_server(const char *host, unsigned short port)
{
	int listenfd;
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		ERR_EXIT("tcp_server");
	

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	int on = 1;
	if ((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on))) < 0)
		ERR_EXIT("setsockopt");

	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");
	    
	if (listen(listenfd, SOMAXCONN) < 0)
		ERR_EXIT("listen");

	return listenfd;
}


	//读取完buf之后，继续读取 
/*
ssize_t read(int fd,void *buf,size_t count)
当offset处于文件的末尾时，返回0
*/
ssize_t readn(int fd,void *buf,size_t count)
{
	size_t nleft = count;//剩余的字符数
	ssize_t nread;
	char *bufp = (char *)buf;//读取字符缓冲区为char *类型
	while(nleft >0)
	{
		if((nread = read(fd,bufp,nleft))<0) //读取异常
		{//POSIX allows a read() that is interrutped after reading
		 // some data to return -1(with errno set to ENTER)
			if(errno == EINTR)
					continue;
			return -1;
		}
		else if(nread == 0)
		{
			return count- nleft;//读完一次返回
		}
		
		bufp += nread;//已经读取的
		nleft-=nread;
		
	}
	return count;
}
/*写入n个字符*/
ssize_t writen(int fd,const void*buf,size_t count)
{
	size_t nleft = count;
	ssize_t nwritten;
	char *bufp = (char *)buf;
	while(nleft > 0)
	{
		if((nwritten =write(fd,bufp,nleft))<0)
		{
			if(errno == EINTR)
				continue;
			return -1;
		}
		else if(nwritten == 0)
		{
			continue;
		}
		buf += nwritten;
		nleft -=nwritten;
	} 
	return count;
}
/*
return data from the beginning of the receive queue
 without removing that data frm quque;
*/
ssize_t recv_peek(int sockfd,void *buf,size_t len)
{
	while(1)
	{
		int ret = recv(sockfd,buf,len,MSG_PEEK);
		if(ret == -1 && errno == EINTR)
			continue;
		return ret;
	}

}
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
	int ret;
	int nread;
	char *bufp = buf;
	int nleft = maxline;
	while (1) 
	{
		ret = recv_peek(sockfd, bufp, nleft);
		if (ret < 0)
			return ret;
		else if (ret == 0)
			return ret;

		nread = ret;
		int i;
		for (i=0; i<nread; i++)
		{
			if (bufp[i] == '\n')
			{
				ret = readn(sockfd, bufp, i+1);
				if (ret != i+1)
					exit(EXIT_FAILURE);

				return ret;
			}
		}

		if (nread > nleft)
			exit(EXIT_FAILURE);

		nleft -= nread;
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
			exit(EXIT_FAILURE);

		bufp += nread;
	}

	return -1;

}