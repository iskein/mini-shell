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

extern int globalpathlen;
extern char initpath[];
extern int pidarr[];
extern char* lastbgcomm[];
extern int procarr[];
extern int arr[];
extern int currpid;
extern int back[];

void nonbuiltin(char* com)
{
	int top = 0;
	int processID;
	char* token;
	char* saveptr;
	char* arr[10000];
	token = strtok_r(com," ",&saveptr);
	int i=0;
	while(token != NULL)
	{
		arr[top++] = token;
		token = strtok_r(NULL," ",&saveptr);
	}
	arr[top] = '\0';
	int status;
	pid_t pid,wait_id;
	pid = fork();
	procarr[pid] = 1;
	int flag = 0;
	if(strcmp(arr[top-1],"&") == 0)
		flag = 1;
	else
		flag = -1;
	if(flag == 1)
		arr[top-1] = '\0';
	if(pid == 0)
	{
		int err = execvp(arr[0],arr);
		if(err < 0)
			printf("** Error in running command **\n");
	}
	else if(pid < 0)
	{
		printf("**  Error in forking  **\n");
	}
	else
	{
		if(flag != 1)
		{
			wait(NULL);
			currpid = pid;
			procarr[pid] = 1;
		}
		else
		{
			pidarr[pid] = 1;
			procarr[pid] = 1;
			back[pid] = 1;
			currpid = 0;
			lastbgcomm[pid] = arr[0];
		}
	}
	return ;
}
