#include"common.h"
#include"session.h"

int main(int argc,char *argv[])
{
	if(getuid() != 0)
	{
		fprintf(stderr,"miniFtpd :must be started ad root!\n");
		exit(EXIT_FAILURE);
	}
		
	session_t sess = 
	{
		-1,-1,0,0,0
	};
	//socket()
	//bind()
	//listen()
	int listenfd = tcp_server(NULL,8000);
	int conn;
	struct sockaddr_in addrcli;
	
	socklen_t addrlen = sizeof(addrcli);
	
	pid_t pid;

	//主进程
	while(1)
	{
		if((conn = accept(listenfd,(struct sockaddr*)&addrcli,(socklen_t * __restrict)&addrlen))<0)
			ERR_EXIT("accept:error");
		if((pid = fork())<0)
			ERR_EXIT("fork:error");
		if(pid ==0)
		{
			close(listenfd);
			sess.ctrl_fd=conn;
			begin_session(&sess);
		}
		else
		{
			close(conn);
		}
	}
	return 0;
}
