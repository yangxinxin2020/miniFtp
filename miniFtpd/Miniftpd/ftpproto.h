#ifndef _FTPPROTO_H_
#define _FTPPROTO_H_
#include"session.h"
#include"str.h"
void ftp_reply(session_t *sess, int status, const char *text);
void handle_child(session_t *sess);
#endif/* _FTPPROTO_H_*/