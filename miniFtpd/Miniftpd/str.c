#include"str.h"
void str_trim_crlf(char *str)
{
	while(*str!='\r' && *str!='\n')
		++str; 
	*str = '\0';
}
//str = "listen_port=8080 "
//listen_port       =     8080
void str_split(const char *str,char *left,char *right,char c)
{
	const char *p = str;
	while(*p != '\0' && *p !=c)
		*p++;
    if(*p == '\0')
	{	
		strcpy(left,str);
	}
	else
	{
		strncpy(left,str,p-str);
		strcpy(right,p+1);
	}
}


int str_all_space(const char *str)
{
	while(*str != '\0' && *str ==' ')
		str++;
	if(*str =='\0')
		return 1;
	return 0;
}