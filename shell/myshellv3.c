#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "PUCITshell:- "

int execute2(char* arglist[]);	//for pipe handling
int execute(char* arglist[],bool isRedirectionOp);
char** tokenize(char* cmdline);
char* read_cmd(char*, FILE*);
int isInternal(char * argList[]);
bool isRedirection(char* argList[]);
void dupCallInRedirection();

char fileNamesToRedirect[2][100];
int numOfPipes=0;
int main(){
   char *cmdline;
   char** arglist;
   char* prompt = PROMPT;   
   while((cmdline = read_cmd(prompt,stdin)) != NULL){
      if((arglist = tokenize(cmdline)) != NULL){
				if(isInternal(arglist)!=1)
				{
					bool isRedirectionOp=isRedirection(arglist);
					if(numOfPipes>0)
						execute2(arglist);
					else
	            	execute(arglist,isRedirectionOp);
				}
       //  need to free arglist
         for(int j=0; j < MAXARGS+1; j++)
	         free(arglist[j]);
         free(arglist);
         free(cmdline);
      }
  }//end of while loop
   printf("\n");
   return 0;
}

void dupCallInRedirection()
{
	if(fileNamesToRedirect[0]!=NULL)
	{
		int fd=open(fileNamesToRedirect[0],O_RDWR);
		dup2(fd,0);	
	}

	if(fileNamesToRedirect[1]!=NULL)
	{
		int fd=open(fileNamesToRedirect[1],O_RDWR);
		dup2(fd,1);	
	}
	return;
}

bool isRedirection(char* argList[])
{
	int indexRow;
	bool isRedirectionOp=false;
	int i=0;
	while(argList[i]!=NULL)
	{
		if(strcmp(argList[i],"|")==0)
			numOfPipes++;		//count number of pipes
		if(strcmp(argList[i],"<")==0 || strcmp(argList[i],">")==0)
		{
			if(strcmp(argList[i],"<")==0)
				indexRow=0;
			if(strcmp(argList[i],">")==0)
				indexRow=1;
			isRedirectionOp=true;
			char* cp = argList[i+1]; // pos in string
			char* start=cp;
			int indexCol=0;		
			while(*cp != ' ' && *cp != '\t' && *cp != '\0')
			{
				fileNamesToRedirect[indexRow][indexCol]= *cp;
				cp++;
				indexCol++;
			}
			fileNamesToRedirect[indexRow][++indexCol] = '\0';
		}
		i++;
	}
	//printf("%s\n",fileNamesToRedirect[0]);
//	printf("%s\n",fileNamesToRedirect[1]);
	return isRedirectionOp;
}

int isInternal(char* argList[])
{
	if(strcmp(argList[0],"exit")==0)
		exit(0);	
	if(strcmp(argList[0],"cd")==0)
	{
		if(chdir(argList[1])!=0)
			printf("bash: cd: %s: No such file or directory\n",argList[1]);
		return 1;
	}
	if(strcmp(argList[0],"help")==0)
	{
		printf("Muneeb Malik's Shell\n");
		printf("The following are built in:\n");  
		printf("cd [Change directory]\nexit [terminate shell program]\nhelp [To get help about Builtin Commands]\n");  
		printf("Use the man -k command for information on other programs.\n");
		return 1;
	}
	return 0;
}

int execute(char* arglist[],bool isRedirectionOp){
   int status;	
	int cpid=fork();
		switch(cpid){
			case -1:
			   perror("fork failed");
				exit(1);
			case 0:				
				if(isRedirectionOp)
				{
					dupCallInRedirection();
					char *tempList[2];
					tempList[0]=arglist[0];
					tempList[1]='\0';
					execvp(arglist[0], tempList);
				}
				else
					execvp(arglist[0], arglist);
	 	      perror("Command not found...");
				exit(1);
			default:
				waitpid(cpid, &status, 0);
		}
  return 0;
}

int execute2(char* arglist[]){
	int status;
   int fd1[2];
   int fd2[2];
   
	char *tempList[2];
	if(numOfPipes==1)
	{
		pipe(fd1);
		if(fork()==0)
		{
			dup2(fd1[1],1);
			close(fd1[0]);
			tempList[0]=arglist[0];
			tempList[1]=arglist[1];
			tempList[2]='\0';
			execvp(tempList[0], tempList);
		}
		else
		{
			dup2(fd1[0],0);
			close(fd1[1]);
			tempList[0]=arglist[3];
			tempList[1]='\0';
			execvp(tempList[0], tempList);
		}
		close(fd1[0]);
		close(fd1[1]);
		for(int i=0;i<2;i++)
			wait(NULL);
	}
	else
	{
		pipe(fd1);
	   pipe(fd2);
		//Parent do a first fork to execute man
		if (fork() == 0){
			dup2(fd1[1], 1);//redirect output of man
			close(fd1[0]);
			close(fd2[0]);
			close(fd2[1]);
			tempList[0]=arglist[0];
			tempList[1]=arglist[1];
			tempList[2]='\0';
			execvp(tempList[0], tempList);
			//execlp("man","man", "ls", NULL);
		}
		//Parent do a second fork to execute grep
		else if (fork() == 0){
			dup2(fd1[0], 0);//redirect stdin of grep
			dup2(fd2[1], 1);//redirect stdout of grep
			close(fd1[1]);
			close(fd2[0]);
			tempList[0]=arglist[3];
			tempList[1]=arglist[4];
			tempList[2]='\0';
			execvp(tempList[0], tempList);
			//execlp("grep","grep","ls", NULL);
		}
		//Parent do a third fork to execute wc
		else if (fork() == 0){
			dup2(fd2[0], 0);//redirect stdin of wc
			close(fd1[0]);
			close(fd1[1]);
			close(fd2[1]);
			tempList[0]=arglist[6];
			tempList[1]='\0';
			execvp(tempList[0], tempList);
			//execlp("wc","wc", NULL);
		}
		//Now the parent waits for three children
		close(fd1[0]);
		close(fd1[1]);
		close(fd2[0]);
		close(fd2[1]);
		for(int i=0;i<3;i++)
			wait(NULL);
	}
	return 0;
}


char** tokenize(char* cmdline){
//allocate memory
   char** arglist = (char**)malloc(sizeof(char*)* (MAXARGS+1));
   for(int j=0; j < MAXARGS+1; j++){
	   arglist[j] = (char*)malloc(sizeof(char)* ARGLEN);
      bzero(arglist[j],ARGLEN);
    }
   if(cmdline[0] == '\0')//if user has entered nothing and pressed enter key
      return NULL;
   int argnum = 0; //slots used
   char*cp = cmdline; // pos in string
   char*start;
   int len;
   while(*cp != '\0'){
      while(*cp == ' ' || *cp == '\t') //skip leading spaces
          cp++;
      start = cp; //start of the word
      len = 1;
      //find the end of the word
      while(*++cp != '\0' && !(*cp ==' ' || *cp == '\t'))
         len++;
      strncpy(arglist[argnum], start, len);
      arglist[argnum][len] = '\0';
      argnum++;
   }
   arglist[argnum] = NULL;
   return arglist;
}      

char* read_cmd(char* prompt, FILE* fp){
   printf("%s", prompt);
  int c; //input character
   int pos = 0; //position of character in cmdline
   char* cmdline = (char*) malloc(sizeof(char)*MAX_LEN);
   while((c = getc(fp)) != EOF){
       if(c == '\n')
	  break;
       cmdline[pos++] = c;
   }
//these two lines are added, in case user press ctrl+d to exit the shell
   if(c == EOF && pos == 0) 
      return NULL;
   cmdline[pos] = '\0';
   return cmdline;
}
