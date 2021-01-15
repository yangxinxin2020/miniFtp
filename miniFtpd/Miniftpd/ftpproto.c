#include"ftpproto.h"
#include"ftpcodes.h"
//ftp服务进程
void ftp_reply(session_t *sess,int status,const char *text)
{
	char buf[MAX_BUFFER_SIZE];
	memset(buf,0,sizeof(buf));
	sprintf(buf,"%d %s\r\n",status,text);
	writen(sess->ctrl_fd,buf,strlen(buf));
    
}
static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_syst(session_t *sess);
static void do_feat(session_t *sess);
static void do_pwd(session_t *sess);
static void do_type(session_t *sess);
static void do_port(session_t *sess);
static void do_pasv(session_t *sess);
/*static void do_cwd(session_t *sess);
static void do_cdup(session_t *sess);
static void do_quit(session_t *sess);
static void do_port(session_t *sess);
static void do_pasv(session_t *sess);
static void do_type(session_t *sess);
//static void do_stru(session_t *sess);
//static void do_mode(session_t *sess);
static void do_retr(session_t *sess);
static void do_stor(session_t *sess);
static void do_appe(session_t *sess);
static void do_list(session_t *sess);
static void do_nlst(session_t *sess);
static void do_rest(session_t *sess);
static void do_abor(session_t *sess);
static void do_pwd(session_t *sess);
static void do_mkd(session_t *sess);
static void do_rmd(session_t *sess);
static void do_dele(session_t *sess);
static void do_rnfr(session_t *sess);
static void do_rnto(session_t *sess);
static void do_site(session_t *sess);
static void do_syst(session_t *sess);
static void do_feat(session_t *sess);
static void do_size(session_t *sess);
static void do_stat(session_t *sess);
static void do_noop(session_t *sess);
static void do_help(session_t *sess);*/
//命令映射结构体
// 函数指针
typedef struct ftpcmd
{
	const char *cmd;
	void (*cmd_handler)(session_t *sess);
}ftpcmd_t;

static ftpcmd_t ctrl_cmds[] = 
{
	{"USER",do_user},
	{"PASS",do_pass},
	{"SYST",do_syst},
	{"FEAT",do_feat},
	{"PWD",do_pwd},
	{"TYPE", do_type},
	{"PORT",do_port},
	{"PASV",do_pasv}	
};


void handle_child(session_t *sess)
{
	//write(sess->ctrl_fd,"200(miniFtpd v 1.0)\r\n",strlen("200(miniFtpd v 1.0)\r\n"));
	ftp_reply(sess,FTP_GREET,"(miniFtpd v 1.0)");
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
		//去除空格
		str_trim_crlf(sess->cmdline);
		
		printf("\n************\nsess:cmdline = [%s]\n", sess->cmdline);
		//解析
		str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
		printf("sess:>>>>command = [%s]\n", sess->cmd);
		printf("sess:arg = [%s]\n************\n", sess->arg);
		
		
		//执行命令
		size_t n = sizeof(ctrl_cmds)/sizeof(ftpcmd_t);
		size_t i;
		for(i = 0;i<n;++i)
		{
			if(strcmp(ctrl_cmds[i].cmd,sess->cmd)==0)
			{//找到命令执行完毕之后结束魂环
				if(ctrl_cmds[i].cmd_handler !=	NULL)
				{	
					(*ctrl_cmds[i].cmd_handler)(sess);
					
				}
				else
 				{	
					char buf[1024]={0};
					sprintf(buf,"%s not implemented.",sess->cmd);
					ftp_reply(sess,FTP_COMMANDNOTIMPL,buf);
				}
				break;
			}
			
		}
		if (i>=n)
		{	
			ftp_reply(sess,FTP_BADCMD,"Unknown command.");
		}
	}
}


/*****
用户鉴权：
给出用户名称->获得用户uid
用户存在->获得用户shdow文件
*****/
// 331 Please specify the password.
//
//           struct passwd {
//               char   *pw_name;       /* username */
//               char   *pw_passwd;     /* user password */
//               uid_t   pw_uid;        /* user ID */
//               gid_t   pw_gid;        /* group ID */
//               char   *pw_gecos;      /* user information */
//               char   *pw_dir;        /* home directory */
//               char   *pw_shell;      /* shell program */
//           };
//  struct passwd *getpwnam(const char *name);
static void do_user(session_t *sess)
{	//根据名称获取密码
	struct passwd  *pw = getpwnam(sess->arg);
	if(pw != NULL)
	{	
		sess->uid= pw->pw_uid;
		//331 Please specify the password.
	}
	ftp_reply(sess,FTP_GIVEPWORD,"Please specify the password.");
	
}
//230 Login successful.
// struct passwd *getpwuid(uid_t uid);

//psss 
// struct spwd {
//               char *sp_namp;     /* Login name */
//               char *sp_pwdp;     /* Encrypted password */
//               long  sp_lstchg;   /* Date of last change
//                                     (measured in days since
//                                     1970-01-01 00:00:00 +0000 (UTC)) */
//               long  sp_min;      /* Min # of days between changes */
//               long  sp_max;      /* Max # of days between changes */
//               long  sp_warn;     /* # of days before password expires
//                                    to warn user to change it */
//               long  sp_inact;    /* # of days after password expires
//                                    until account is disabled */
//               long  sp_expire;   /* Date when account expires
//                                    (measured in days since
//                                   1970-01-01 00:00:00 +0000 (UTC)) */
//               unsigned long sp_flag;  /* Reserved */
//         };
static void do_pass(session_t *sess)
{	
	struct passwd  *pw = getpwuid(sess->uid);
	if(pw == NULL)
	{
		ftp_reply(sess,FTP_LOGINERR,"Login incorrect.");
		return ;
	}//根据用户名得到shadow文件加密后的内容

	
	//struct spwd *getspnam(const char *name);
	struct spwd *spw = getspnam(pw->pw_name);
	//用户存在，获取到了shdow密码，接着匹配密码
	//printf("\npasswd = [[%s]]\n",spw->sp_pwdp);

	// char *crypt(const char *key, const char *salt);
	char *crypt_passwd = crypt(sess->arg,spw->sp_pwdp);
	if(strcmp(spw->sp_pwdp,crypt_passwd) != 0)
	{
		ftp_reply(sess,FTP_LOGINERR,"Login incorrect.");
		return ;
	}
    //设置FTP服务进程,把进程更改为当前的用户名
//               1、gid_t   pw_gid;        /* group ID */
//               2、uid_t   pw_uid;        /* user ID */
//				 3、char   *pw_dir;        /* home directory */
	setegid(pw->pw_gid);
	seteuid(pw->pw_uid);
	chdir(pw->pw_dir);

	//设置
	//230 Login successful.
	ftp_reply(sess,FTP_LOGINOK,"Login sucessful.");
	
}
// 215 UNIX Type: L8
static void do_syst(session_t *sess)
{
	ftp_reply(sess,FTP_SYSTOK,"UNIX Type: L8");
}

/*
[15:01:31] 211-Features:
[15:01:31]  EPRT
[15:01:31]  EPSV
[15:01:31]  MDTM
[15:01:31]  PASV
[15:01:31]  REST STREAM
[15:01:31]  SIZE
[15:01:31]  TVFS
[15:01:31]  UTF8
[15:01:31] 211 End

*/
static void do_feat(session_t *sess)
{

	 writen(sess->ctrl_fd, "211-Features:\r\n", strlen("211-Features:\r\n"));
	 writen(sess->ctrl_fd, " EPRT\r\n", strlen(" EPRT\r\n"));
	 writen(sess->ctrl_fd, " EPSV\r\n", strlen(" EPSV\r\n"));
	 writen(sess->ctrl_fd, " MDTM\r\n", strlen(" MDTM\r\n"));
	 writen(sess->ctrl_fd, " PASV\r\n", strlen(" PASV\r\n"));
	 writen(sess->ctrl_fd, " REST STREAM\r\n", strlen(" REST STREAM\r\n"));
	 writen(sess->ctrl_fd, " SIZE\r\n", strlen(" SIZE\r\n"));
	 writen(sess->ctrl_fd, " TVFS\r\n", strlen(" TVFS\r\n"));
	 writen(sess->ctrl_fd, " UTF8\r\n", strlen(" UTF8\r\n"));
	 ftp_reply(sess, FTP_FEAT, "end");

}

//当前的工作目录
static void do_pwd(session_t *sess)
{
	char text[1024] = {0};
	char dir[1024+1] = {0};
//char *getcwd(char *buf, size_t size);	
	getcwd(dir,1024);
	sprintf(text,"\"%s\"",dir);
	ftp_reply(sess, FTP_PWDOK, text);
	
}

static void do_type(session_t *sess)
{
	//A
	// 200 Switching to ASCII mode.
	if(strcmp(sess->arg, "A") == 0)
	{
		sess->is_ascii = 1;
		ftp_reply(sess, FTP_TYPEOK, "Switching to ASCII mode.");
	}
	//I
	//200 Switching to Binary mode.
	else if(strcmp(sess->arg, "I") == 0)
	{
		sess->is_ascii = 0;
		ftp_reply(sess, FTP_TYPEOK, "Switching to Binary mode.");
	}
	//500 Unrecognised TYPE command.
	else
	{
		ftp_reply(sess, FTP_BADCMD, "Unrecognised TYPE command.");
	}

}
//PORT 192,168,31,250,200,124
//command:PORT
//arg:192,168,31,250,200,124
// int scanf(const char *format, ...);
// int fscanf(FILE *stream, const char *format, ...);
// int sscanf(const char *str, const char *format, ...);
//ip 端口号解析到结构当中
/*
1、客户端向服务器发送PORT命令
     客户端创建数据套接字
     客户端在套接字上面监听
    将IP与端口格式化为h1,h2,h3,h4,p1,p2
2、服务器端以200响应
    服务器端解析客户端发过来的IP与端口暂时封存起来，以便以后继续建立数据连接。
3、客户端向服务器发送LIST
   服务器端检测在收到LIST命令之前是否接收到PORT或者PASV命令
  如果没有接收过，则响应425 Use PORT or PASV命令
  如果有接收过，而且是PORT，则服务器端创建数据套接字（bind 20 端口），调用connect主动连接客户端IP与端口，从而建立数据连接
4、服务器发送150应答给客户端，表示准备就绪，可以开始传输了。150 Here comes the directory listing.
5、开始传输列表
6、服务器发送226Directory send OK

*/         
static void do_port(session_t *sess)
{
	unsigned v[6] = {0};
	sscanf(sess->arg,"%d,%d,%d,%d,%d,%d",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5]);
	//printf("v[0] = %d,v[1] = %d,v[2] = %d,v[3] = %d,v[4] = %d,v[5] = %d\n",v[0],v[1],v[2],v[3],v[4],v[5]);
	sess->port_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(sess->port_addr,0,sizeof(struct sockaddr_in));
  
    sess->port_addr->sin_family = AF_INET;
	//将ip地址分割为四部分
	unsigned char *p = (unsigned char*)&sess->port_addr->sin_addr.s_addr;
	
	p[0] = v[0];
	p[1] = v[1];
	p[2] = v[2];
	p[3] = v[3];
	//将端口号分割为两部分
	p = (unsigned char *)&sess->port_addr->sin_port;
	
	p[0] = v[4];
	p[1] = v[5];

	printf("ip = %s\n",inet_ntoa(sess->port_addr->sin_addr));
	printf("port = %d\n",ntohs(sess->port_addr->sin_port));
	//1100100101010001  
	//1100100101010001 
}
//227 Entering Passive Mode (192,168,31,67,117,22).
/*
typedef struct session
{
	//控制连接
	uid_t uid;
	int ctrl_fd;
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char arg[MAX_ARG];
	//数据连接
	struct sockaddr_in *port_addr;
	int pasv_fd;
	//协议状态
	int is_ascii;
}session_t;

*/
static void do_pasv(session_t *sess)
{
	 char *ip ="192.168.31.67";
	//socket()
	//bind()
	//listen()
	sess->pasv_fd = tcp_server(ip,0);
	socklen_t socklen = sizeof(sess->pasv_addr);
	
    //根据描述符获取套接字信息
	if(getsockname(sess->pasv_fd,(struct sockaddr *)&sess->pasv_addr,&socklen)<0)
		ERR_EXIT("getsockname");

	printf("ip = %s\n",inet_ntoa(sess->pasv_addr.sin_addr));
	printf("port = %d\n",ntohs(sess->pasv_addr.sin_port));

    unsigned short port = ntohs(sess->pasv_addr.sin_port);
	unsigned char adres[6]={0};
	sscanf(ip, "%u.%u.%u.%u",&adres[0],&adres[1],&adres[2],&adres[3]);
	adres[4] = ((port>>8) & 0x00ff);
	adres[5] = port & 0x00ff;
	char buf[MAX_BUFFER_SIZE] = {0};
	sprintf(buf,"Entering Passive Mode (%u,%u,%u,%u,%u,%u)",adres[0],adres[1],adres[2],adres[3],adres[4],adres[5]);
	ftp_reply(sess, FTP_PASVOK,buf);
}
