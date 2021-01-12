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
	{"PASS",do_pass}	
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