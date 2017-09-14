#include"builtin.h"
#include"nonbuiltin.h"
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
#include<fcntl.h>


int globalpathlen = 0;
char initpath[1000];
int pidarr[100000];
char* lastbgcomm[1000000];
int procarr[100005];
int back[100005];
int arr[2];
int currpid = 0;
int stdin_copy;
int stdout_copy;



void execute(char *command)
{
	int i = 0;
	arr[0] = 0;
	arr[1] = 0;
	char filepath[1024];
	char filename[1024];
	char tempc[1024];
	int len = strlen(command);
	for(i=0;i<len;i++)
		if((int)command[i] == 9)
			command[i] = (char)(32);
	for(i=0;i<len;i++)
		tempc[i] = command[i];
	tempc[len] = '\0';
	char* filetoken;
	char* fileptr;
	char *part;
	char* saveptr;
	for(i=0;i<len;i++)
	{
		if(command[i] == 60) // <
			arr[0] += 1;
		if(command[i] == 62) // > 
			arr[1] += 1;
	}
	char* commandptr;
	char* comm;
	comm = tempc;
	if(arr[0] == 1 && arr[1] == 0)
	{
		filetoken = strtok_r(tempc,"<",&fileptr);
		while(*fileptr == 32)
			fileptr++;
		for(i=0;i<strlen(fileptr);i++)
			if(fileptr[i] == 32)
			{
				fileptr[i] = '\0';
				break;
			}
		getcwd(filepath,1000);
		sprintf(filepath,"%s/%s",filepath,fileptr);
		comm = strtok_r(command,"<",&commandptr);
		int fd = open(filepath,O_RDONLY);
		dup2(fd,0);
		close(fd);
	}
	else if(arr[0] == 0 && arr[1] >= 1)
	{
		if(arr[1] == 1)
		{
			filetoken = strtok_r(tempc,">",&fileptr);
			while(*fileptr == 32)
				fileptr++;
			for(i=0;i<strlen(fileptr);i++)
				if(fileptr[i] == 32)
				{
					fileptr[i] = '\0';
					break;
				}
			getcwd(filepath,1000);
			sprintf(filepath,"%s/%s",filepath,fileptr);
			comm = strtok_r(command,">",&commandptr);
			int fd = open(filepath,O_WRONLY | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO);
			dup2(fd,1);
			close(fd);
		}
		if(arr[1] == 2)
		{
			filetoken = strtok_r(tempc,">",&fileptr);
			while(*fileptr == 32 || *fileptr == 62)
				fileptr++;
			for(i=0;i<strlen(fileptr);i++)
				if(fileptr[i] == 32)
				{
					fileptr[i] = '\0';
					break;
				}
			getcwd(filepath,1000);
			sprintf(filepath,"%s/%s",filepath,fileptr);
			comm = strtok_r(command,">",&commandptr);
			int fd = open(filepath,O_WRONLY | O_APPEND | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO);
			dup2(fd,1);
			close(fd);

		}
	}
	else if(arr[0] == 1 && arr[1] >= 1)
	{
		char* in;
		char* out;
		char* inptr;
		char* outptr;
		char inpath[1024];
		char outpath[1024];
		comm = strtok_r(command,"<",&commandptr);
		if(arr[1] == 1)
		{
			in = strtok_r(commandptr,">",&inptr);
			while(*in == 32)
				in++;
			for(i=0;i<strlen(in);i++)
				if(in[i] == 32)
				{
					in[i] = '\0';
					break;
				}
			out = inptr;
			while(*out == 32)
				out++;
			for(i=0;i<strlen(out);i++)
				if(out[i] == 32)
				{
					out[i] = '\0';
					break;
				}
			getcwd(inpath,1024);
			getcwd(outpath,1024);
			sprintf(inpath,"%s/%s",inpath,in);
			sprintf(outpath,"%s/%s",outpath,out);
			int fd1 = open(outpath,O_WRONLY | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO);
			int fd2 = open(inpath,O_RDONLY);
			dup2(fd1,1);
			dup2(fd2,0);
			close(fd1);
			close(fd2);
		}
		if(arr[1] == 2)
		{
			in = strtok_r(commandptr,">>",&inptr);
			while(*in == 32)
				in++;
			for(i=0;i<strlen(in);i++)
				if(in[i] == 32)
				{
					in[i] = '\0';
					break;
				}
			out = inptr;
			while(*out == 32 || *out == 62)
				out++;
			for(i=0;i<strlen(out);i++)
				if(out[i] == 32)
				{
					out[i] = '\0';
					break;
				}
			getcwd(inpath,1024);
			getcwd(outpath,1024);
			sprintf(inpath,"%s/%s",inpath,in);
			sprintf(outpath,"%s/%s",outpath,out);
			int fd1 = open(outpath,O_WRONLY | O_APPEND | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO);
			int fd2 = open(inpath,O_RDONLY);
			dup2(fd1,1);
			dup2(fd2,0);
			close(fd1);
			close(fd2);
		}
	}
	char temp[1024];
	part = strtok_r(command," ",&saveptr);
	if(strcmp(part,"cd") == 0)
		cd(comm);
	else if(strcmp(part,"pwd") == 0)
		pwd(comm);
	else if(strcmp(part,"echo") == 0)
		echo(comm);
	else if(strcmp(part,"pinfo") == 0)
		pinfo(comm);
	else if(strcmp(part,"jobs") == 0)
		jobs(comm);
	else if(strcmp(part,"sendsig") == 0)
		sendsig(comm);
	else if(strcmp(part,"exit") == 0)
		exit(0);
	else if(strcmp(part,"killallbg") == 0)
		killallbg();
	else if(strcmp(part,"fg") == 0)
		fg(comm);
	else
		nonbuiltin(comm);

	return ;
}

/*
void piping(char* command)
{
	int i=0;
	int pipefds[1024];
	int cnt = 0;
	for(i=0;i<strlen(command);i++)
	{
		if(command[i] == (int)'|')
			cnt += 1;
	}
	for(i=0;i<cnt;i++)
	{
		if(pipe(pipefds + i*2) < 0)
			perror("Error creating pipe");
	}
	int commandc = 0;
	char* token;
	char* saveptr;
	token = strtok_r(command,"|",&saveptr);
	while(token)
	{
		if(commandc == 0)
		{
			if(commandc + 1 <= cnt)
			{

				if(dup2(pipefds[1],1) < 0)
				{
					perror("Error : piping\n");
					break;
				}
				close(pipefds[1]);

			}
		}
		else if(commandc == cnt)
		{

			if(dup2(pipefds[(commandc-1)*2],0) < 0)
			{
				perror("Error : piping\n");
				break;
			}
			close(pipefds[(cnt-1)*2]);

		}
		else if(commandc-1 >= 0 && commandc+1 <= cnt)
		{

			if(dup2(pipefds[(commandc-1)*2],0) < 0)
			{
				perror("Error : piping\n");
				break;
			}
			if(dup2(pipefds[commandc*2 + 1],1) < 0)
			{
				perror("Error : piping\n");
				break;
			}
			close(pipefds[(commandc-1)*2]);
			close(pipefds[commandc*2+1]);

		}
		execute(token);
		token = strtok_r(NULL,"|",&saveptr);
		commandc += 1;
	}
	for(i=0;i<2*commandc;i++)
		close(pipefds[i]);
}
*/

void parse(char* command)
{
	char* saveptr;
	char temp1[1000];
	strcpy(temp1,command);
	char *token;
	token = strtok_r(temp1,";",&saveptr);
	while(token!=NULL)
	{
		dup2(stdin_copy,0);
		dup2(stdout_copy,1);
		//piping(token);
		execute(token);
		token = strtok_r(NULL,";",&saveptr);
	}
	return ;
}

// handler functions
static void handler(int signu, siginfo_t* siginfo, void* context)
{
	if(pidarr[siginfo->si_pid] == 1)
	{
		procarr[siginfo->si_pid] = 0;
		if((int)(siginfo->si_errno) > 0)
			printf("%s with pid %d  exited with error %s\n",lastbgcomm[siginfo->si_pid],(int)(siginfo->si_pid),strerror(siginfo->si_errno));
		else
			printf("%s with pid %d exited normally\n",lastbgcomm[siginfo->si_pid],(int)(siginfo->si_pid));
		pidarr[siginfo->si_pid] = 0;
	}
}
void sigint_handler(int sig) // ctrl+c
{
	dup2(stdin_copy,0);
	dup2(stdout_copy,1);
	printf("SIGINT recieved\n");
	procarr[currpid] = 0;
	kill(currpid,SIGINT);
}
void sigtstp_handler(int sig) // ctrl+z
{
	dup2(stdin_copy,0);
	dup2(stdout_copy,1);
	printf("SIGTSTP recieved\n");
	kill(currpid,SIGCONT);
	procarr[currpid] = 1;
	back[currpid] = 1;
	currpid = 0;
}

int pathflag = -1;
int main()
{
	signal(SIGINT,sigint_handler);
	signal(SIGTSTP,sigtstp_handler);
	struct sigaction sas1;
	memset(&sas1,0,sizeof(sas1));
	sas1.sa_sigaction = &handler;
	sas1.sa_flags=SA_SIGINFO;
	if(sigaction(SIGCHLD,&sas1,NULL)<0)
		perror("Sigaction");
	stdin_copy = dup(0);
	stdout_copy = dup(1);
	int k;
	for(k=0;k<100005;k++)
	{
		procarr[k] = 0;
		back[k] = 0;
	}
	char hostname[1000];
	char path[10000];
	char domainname[1000];
	int hostnameint = gethostname(hostname,1000);
	struct passwd *username;
	int pathlen = 0;
	getcwd(initpath,1000);
	username = getpwuid(geteuid());
	getcwd(path,10000);
	for(pathlen = 0;pathlen<10000;pathlen++)
	{
		if((int)path[pathlen] == 0)
			break;
	}
	if(hostnameint == -1)
	{
		printf("ERROR: hostname could not be retrived\n");
	}
	char command[1000];
	globalpathlen = pathlen;
	int i;
	while(1)
	{
		dup2(stdin_copy,0);
		dup2(stdout_copy,1);
		getcwd(path,10000);
		int newpathlen = 0;
		for(newpathlen=0;newpathlen<10000;newpathlen++)
			if((int)path[newpathlen] == 0)
				break;
		if(globalpathlen <= newpathlen)
		{
			printf("%s@%s:~",username->pw_name,hostname);
			for(i=pathlen;i<newpathlen;i++)
				printf("%c",path[i]);
			printf(">");
		}
		else
		{
			printf("%s@%s:%s>",username->pw_name,hostname,path);
		}
		fscanf(stdin," %[^\n]s",command);
		if(command[0] == 0)
		{
			printf("\n");
			exit(0);
		}
		parse(command);
		strcpy(command,"");
	}
	return 0;
}
