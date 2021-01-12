#include"ftpproto.h"
//ftp服务进程
void ftp_reply(session_t *sess,int status,const char *text)
{
	char buf[MAX_BUFFER_SIZE];
	memset(buf,0,sizeof(buf));
	sprintf(buf,"%d%s\r\n",status,text);
	writen(sess->ctrl_fd,buf,strlen(buf));
    
}
void handle_child(session_t *sess)
{
	//write(sess->ctrl_fd,"200(miniFtpd v 1.0)\r\n",strlen("200(miniFtpd v 1.0)\r\n"));
	ftp_reply(sess,200,"(miniFtpd v 1.0)");
	int ret;//if ret is 0,maybe client off
	//
	while(1)
	{	//清零
		memset(sess->cmdline,0,sizeof(sess->cmdline));
		memset(sess->cmd,0,sizeof(sess->cmd));
		memset(sess->arg,0,sizeof(sess->arg));
		ret = readline(sess->ctrl_fd,sess->cmdline,MAX_COMMAND_LINE);
		if(ret == 0)
		{}
		else if(ret <0)
		{}
		str_trim_crlf(sess->cmdline);
		printf("sess:cmdline = [%s]\n", sess->cmdline);
		str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
		printf("sess:command = [%s]\n", sess->cmd);
		printf("arg = [%s]\n", sess->arg);
		
		
		
	}
}