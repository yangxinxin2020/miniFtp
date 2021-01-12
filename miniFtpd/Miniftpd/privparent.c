#include"privparent.h"
//nobody进程
void handle_parent(session_t *sess)
{
	//发送欢迎信息
	//send(sess->ctrl_fd,"200(miniFtpd v 1.0)\r\n",strlen("200(miniFtpd v 1.0)\r\n"),0);
	//write(sess->ctrl_fd,"200(miniFtpd v 1.0)\r\n",strlen("200(miniFtpd v 1.0)\r\n"));
	while(1)
	{
		
	}
} 


