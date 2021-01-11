#include"session.h"

void begin_session(session_t *sess)
{
	pid_t pid = fork();
	if(pid < 0 )
		ERR_EXIT("begin_session:fork");
	if(pid == 0)
	{
		//FTP服务进程
		handle_child(sess);
	}
	else
	{
		//nobody进程
		handle_parent(sess);
	}
}