#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>
#include <sys/unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#define vee_rl 1024
#define vee_t 64
#define vee_delim " \t\r\n\a"
#define vee_deli ";"
#define vee_del "|"
#define vee_de "="
char **env1;
char start_pwd[1024];
int cat=0;
char *getlogin(void);
int cd(char **array);
int v_exit(char **array);
int pwd(char **array);
int echo(char **array);
int spawn_proc(int in,int out,char **array);
int piping(char **array);
char *command_str[]={"cd","exit","pwd","echo"};
char **split_li(char *line,int i);
int flag_backgroud=0;
int (*builtin_func[])(char **)={
	&cd,
	&v_exit,
	&pwd,
	&echo
};
int vee_d=sizeof(command_str)/sizeof(char *);
int echo(char **array)
{

	//if(pid==0)
	//{

	if(array[1][0]!='$')
	{
		pid_t pid;
		pid=fork();
		if(pid==0)
		{
			//	printf("$$$$$$$$$$\n");
			int i=1,j,l;
			while(array[i+1]!=NULL)
			{
				j=0;
				l=strlen(array[i]);
				while(j<l)
				{
					if(array[i][j]!='\"')// ||(j==0 ||(j>0 && array[i][j-1]!='\\')))
						printf("%c",array[i][j]);
					j++;
				}
				i++;
				printf(" ");
			}
			j=0;
			l=strlen(array[i]);
			while(j<l)
			{
				if(array[i][j]!='\"')
					printf("%c",array[i][j]);
				j++;
			}
			printf("\n");
			//	wait(0);
			//	printf("$$$$$$$$$$\n");
			exit(1);
		}
	}
	else
	{
		//printf("$$$$$$$$$$\n");
		int i=1;
		char vee[10000];
		char **arr;
		while(array[1][i]!='\0')
		{
			vee[i-1]=array[1][i];
			i++;
		}
		i=0;
		//			sleep(1);
		while(env1[i]!=NULL)
		{
			arr=split_li(env1[i],1);
			if(strcmp(arr[0],vee)==0)
				printf("%s\n",arr[1]);
			i++;
		}
	}
	//}
	sleep(1);
	return 1;

}
int cd(char **array)
{
	if (array[1]==NULL)
		fprintf(stderr,"lsh: expected arguement to \"cd\"\n");
	else
		if(chdir(array[1])!=0)
			perror("lsh");
	return 1;
}
int pwd(char **array)
{
	char hostname[128],x[1024];
	getcwd(x,sizeof x);
	printf("%s\n",x);
}
int v_exit(char **array)
{
	return 0;
}

int run_command(char **array)
{
	//int j=0;
	//while(array[j]!=NULL)
	//{
	//	printf("%s\n",array[j] );
	//	j++;
	//}
	int i;
	if(array[0]==NULL)
		return 1;
	for(i=0;i<vee_d;i++)
		if(strcmp(array[0],command_str[i])==0)
			return (*builtin_func[i])(array);
	return launch(array,-1,-1);
}
int launch(char **array,int in,int ou)
{
	pid_t pid,wpid;
	int status;
	pid=fork();
	if(pid==0)
	{
		if(in>0)
		{
			int inp=open(array[in+1],O_RDONLY,0);
			dup2(inp,fileno(stdin));
			close(inp);
			array[in]='\0';
		}
		if(ou>0)
		{
			int inp=creat(array[ou+1],0644);
			dup2(inp,fileno(stdout));
			close(inp);
			array[ou]='\0';
		}
		if(flag_backgroud==1)
		{
			setpgid(0,0);	
		}
		if(execvp(array[0],array)==-1)
			perror("lsh");
		exit(EXIT_FAILURE);
	}
	else if(pid<0)
	{
		perror("lsh");
	}
	else
	{
		if(flag_backgroud==0)
		{
			do{
				wpid=waitpid(pid,&status,WUNTRACED);
			}while(!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}
	return 1;
}

int var1=-1,var2=-1,in_i=-1,ou_i=-1,in=-1,ou=-1;
char **split_line(char *line)
{
	flag_backgroud=0;
	int bufsize=vee_t,position=0,i=0;
	in=-1;
	ou=-1;
	char **tokens=malloc(bufsize*sizeof(char*));
	char *token;
	token=strtok(line,vee_delim);
	while(token!=NULL)
	{
		tokens[position]=token;
		if (strcmp("<",token)==0)
		{
			in++;
			in_i=i;
		}
		else if (strcmp(">",token)==0)
		{
			ou++;
			ou_i=i;
		}
		if(strcmp("&",token)==0)
		{
			flag_backgroud=1;
			tokens[position]=NULL;
		}
		position++;
		if(position>=bufsize)
		{
			bufsize+=vee_t;
			tokens=realloc(tokens,bufsize*sizeof(char*));
		}
		i++;
		token=strtok(NULL,vee_delim);
	}
	tokens[position]=NULL;
	/*if(in==0|ou==0)
	{
		var1=0;
		launch(tokens,in_i,ou_i);
	}
	/*{
		var1=0;
		inou(in,ou,in_i,ou_i,tokens);
		if(strcmp(tokens[0],"cat")==0)
			sleep(1);
	}
	*/if(in>0|ou>0)
	{
		var2=0;
		printf("syntax error near unexpected token\n");
	}
	return tokens;
}
char **split_lin(char *line)
{
	int bufsize=vee_t,position=0;
	char **tokens=malloc(bufsize*sizeof(char*));
	char *token;
	token=strtok(line,vee_deli);
	while(token!=NULL)
	{
		tokens[position]=token;
		position++;
		if(position>=bufsize)
		{
			bufsize+=vee_t;
			tokens=realloc(tokens,bufsize*sizeof(char*));
		}
		token=strtok(NULL,vee_deli);
	}
	tokens[position]=NULL;
	return tokens;
}
char **split_li(char *line,int i)
{
	int bufsize=vee_t,position=0;
	char **tokens=malloc(bufsize*sizeof(char*));
	char *token;
	if(i==0)
		token=strtok(line,vee_del);
	else if(i==1)
		token=strtok(line,vee_de);
	while(token!=NULL)
	{
		tokens[position]=token;
		position++;
		if(position>=bufsize)
		{
			bufsize+=vee_t;
			tokens=realloc(tokens,bufsize*sizeof(char*));
		}
		if(i==0)
			token=strtok(NULL,vee_del);
		else if(i==1)
			token=strtok(NULL,vee_de);
	}
	tokens[position]=NULL;
	return tokens;
}
int check_array(int position,int bufsize,char *buffer)
{

	if(position>=bufsize)
	{
		bufsize+=vee_rl;
		buffer=realloc(buffer,bufsize);
		if(!buffer)
		{
			fprintf(stderr,"lsh:allocation error\n");
			exit(EXIT_FAILURE);
		}
	}
	return bufsize;
}
char *read_line(void)
{
	flag_backgroud=0;
	int bufsize=vee_rl;
	int position=0;
	char *buffer=malloc(sizeof(char)*bufsize);
	int c;
	if(!buffer){
		fprintf(stderr,"lsh:allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		c=getchar();
		if(c==EOF||c=='\n')//||c=='|'||c==';')
		{
			buffer[position]='\0';
			return buffer;
		}
		else
			buffer[position]=c;
		position++;
		bufsize=check_array(position,bufsize,buffer);
	}

}
int spawn_proc(int in,int out,char **array)
{
	pid_t pid;
	if((pid=fork())==0)
	{
		if(in!=0)
		{
			dup2(in,0);
			close(in);
		}
		if(out!=1)
		{
			dup2(out,1);
			close(out);
		}
		return run_command(array);
	}
	return pid;
}	
int piping(char **array)
{
	int i,n=0,in=0,fd[2];
	pid_t pid;
	while(array[n]!=NULL)
		n++;
	n--;
	for (i = 0; i < n-1; ++i)
	{
		pipe(fd);
		spawn_proc(in,fd[1],array);
		close(fd[1]);
		in=fd[0];
	}
	if(in!=0)
		dup2(in,0);
	return run_command(array);
}
void loop(char **env)//void)
{
	char *line;
	char **array;
	env1=env;
	char **arg;
	char **pip;
	int status;
	char *user=getenv("USER");//to get username
	char hostname[128],x[1024];
	getcwd(start_pwd,sizeof start_pwd);
	int i=0,j;
	while(start_pwd[i]!='\0')
		i++;
	do{
		j=i;
		getcwd(x,sizeof x);
		//	printf("%s@>",user);
		gethostname(hostname,sizeof hostname);
		printf("<%s@%s:~>",user,hostname);
		while(x[j]!='\0')
		{
			printf("%c\n",x[j]);
			j++;
		}
		line = read_line();
		arg=split_lin(line);
		int i=0;
		if(arg[1]!=NULL)
			while(arg[i]!=NULL)
			{
				in_i=-1;
				ou_i=-1;
				pip=split_li(arg[i],0);
				if(pip[1]!=NULL)
				{
					//int j=0;	
					//while(pip[j]!=NULL)
					//{
					//	printf("%s\n",pip[j] );
					//	j++;
					//}
					status=piping(pip);
					//					var1=-1;
					//					array=split_line(arg[i]);
					//					if(var1==-1)
					//						status=run_command(array);					
				}
				else
				{
					var1=-1;
					var2=-1;
					array=split_line(arg[i]);
					//printf("$$$$$$\n");
					//printf("%d%d\n",in_i,ou_i );
					if(in==0|ou==0)
						status=launch(array,in_i,ou_i);
					else
						status=run_command(array);	
				}
				i++;
			}
		else
		{
			var1=-1;
			var2=-1;
			array=split_line(line);
			//printf("$$$$$$1\n");
			if(in==0|ou==0)
					status=launch(array,in_i,ou_i);
			else
					status=run_command(array);	
			//printf("$$$$$$2%d\n",status);
		}
	}while(status);
}

int main(int argc,char **argv,char *env[])
{
	loop(env);
	return EXIT_SUCCESS;
}
