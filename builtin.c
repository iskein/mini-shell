#include<stdio.h>
#include<sys/utsname.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/utsname.h>
#include<pwd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<signal.h>
#include<errno.h>
#include<sys/stat.h>

extern int globalpathlen;
extern char initpath[];
extern int pidarr[];
extern char* lastbgcomm[];
extern int procarr[];
extern int back[];
extern int currpid;

void fg(char* comm)
{
	char* token;
	char* ptr;
	token = strtok_r(comm," ",&ptr);
	token = strtok_r(NULL," ",&ptr);
	int pid = 0;
	int len = strlen(token);
	int i = 0;
	while(i < len)
	{
		pid = pid*10 + token[i]-'0';
		i += 1;
	}
	currpid = pid;
	back[pid] = 0;
	procarr[pid] = 1;
	pidarr[pid] = 1;
	int ret = kill(pid, SIGCONT);
	if(ret == 0)
	{
		printf("%d brought in fg\n",pid);
	}
	else
		printf("%d Not able to bring in fg",pid);
}
void killallbg()
{
	int i;
	for(i=50;i<100005;i++)
	{
		if(back[i] == 1)
			kill(i,SIGKILL);
		back[i] = 0;
	}
	return ;
}
void sendsig(char* com)
{
	char* token;
	char* ptr;
	token = strtok_r(com," ",&ptr);
	token = strtok_r(NULL," ",&ptr);
	int pid = 0;
	int len = strlen(token);
	int i=0;
	while(i<len)
	{
		pid = pid*10 + token[i] - '0';
		i += 1;
	}
	token = strtok_r(NULL," ",&ptr);
	int sig = 0;
	len = strlen(token);
	i = 0;
	while(i < len)
	{
		sig = sig*10 + token[i] - '0';
		i += 1;
	}
	int ret = kill(pid,sig);
	if(ret == 0)
		printf("Signal sent successfully\n");
	else
		printf("Error while sending Signal\n");
}

void jobs(char* com)
{
	char path[1024];
	char line[1024];
	int i = 1;
	struct stat sb;
	int cnt = 1;
	while(i < 100005)
	{
		if(procarr[i] == 1)
		{
			sprintf(path,"/proc/%d/stat",i);
			if(stat(path,&sb) == 0)
			{
				FILE* fptr = fopen(path,"r");
				fscanf(fptr," %[^\n]s",line);
				int len = strlen(line);
				char* linetoken;
				char* lineptr;
				{
					{
						printf("[%d] %d",cnt++,i);
						sprintf(path,"/proc/%d/status",i);
						fptr = fopen(path,"r");
						fscanf(fptr," %[^\n]s",line);
						len = strlen(line);
						int k;
						for(k=0;k<len;k++)
							if(line[k] == 9)
								line[k] = (char)32;
						linetoken = strtok_r(line," ",&lineptr);
						linetoken = strtok_r(NULL," ",&lineptr);
						printf(" %s\n",linetoken);
					}
				}
				fclose(fptr);
			}
		}
		i += 1;
	}
}
void pinfo(char* com)
{
	char* token;
	char* saveptr;
	token = strtok_r(com," ",&saveptr);
	token = strtok_r(NULL," ",&saveptr);
	int len = strlen(token);
	int pid = 0;
	int i= 0;
	while(i<len)
	{
		pid = pid*10 + ((int)token[i] - (int)'0');
		i += 1;
	}
	char line[10000];
	char path[100];
	FILE* fptr;
	sprintf(path,"/proc/%d/status",pid);
	if((fptr = fopen(path,"r")) == NULL){
		printf("Error accessing proc file\n");
		return ;
	}
	while(fscanf(fptr," %[^\n]s",line) == 1)
	{
		char* linetoken;
		char* linesaveptr;
		int len = strlen(line);
		for(i=0;i<len;i++)
			if(line[i] == 9)
				line[i] = (char)32;
		linetoken = strtok_r(line," ",&linesaveptr);
		if(strcmp(linetoken,"Pid:") == 0)
		{
			printf("%s %s\n",linetoken,linesaveptr);
			linetoken = strtok_r(line," ",&linesaveptr);
		}
		if(strcmp(linetoken,"State:") == 0)
		{
			printf("Process state : %s\n",linesaveptr);
			linetoken = strtok_r(line," ",&linesaveptr);
		}
		if(strcmp(linetoken,"VmSize:") == 0)
		{
			printf("Virtual Memory : %s\n",linesaveptr);
			linetoken = strtok_r(line," ",&linesaveptr);
		}
	}
	sprintf(path,"/proc/%d/cmdline",pid);
	if((fptr = fopen(path,"r")) == NULL)
	{
		printf("Error accessing proc files\n");
		return ;
	}
	fscanf(fptr," %[^\n]s",line);
	printf("Executable path : %s\n",line);
	return ;
}
void cd(char* com)
{
	char* token;
	char* saveptr;
	token = strtok_r(com," ",&saveptr);
	token = strtok_r(NULL," ",&saveptr);
	int xlen = strlen(token);
	char currpath[10000];
	getcwd(currpath,10000);
	int len = strlen(currpath);
	int i;
	currpath[len] = '/';
	for(i=0;i<xlen;i++)
		currpath[i+len+1] = token[i];
	currpath[i+len+1] = '\0';
	int err = chdir(currpath);
	if(err == -1)
		printf("No such path exists\n");
	return ;
}

void pwd(char* com)
{
	int i;	
	char temp[1000];
	getcwd(temp,1000);
	int newlen = strlen(temp);
	printf("home/");
	for(i=globalpathlen+1;i<newlen;i++)
		printf("%c",temp[i]);
	printf("\n");
	return ;
}

void echo(char* com)
{
	char* token;
	char* saveptr;
	token = strtok_r(com," ",&saveptr);
	printf("%s\n",saveptr);
	return ;
}


