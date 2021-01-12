#ifndef _SYSUTIL_H_
#define _SYSUTIL_H_

#include"common.h"
int tcp_server(const char *host, unsigned short port);
ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd, void *buf, size_t len);
ssize_t readline(int sockfd, void *buf, size_t maxline);
#endif /*_SYSUTIL_H_*/