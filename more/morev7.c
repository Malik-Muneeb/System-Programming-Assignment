#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define LINELEN 512


int PAGELEN=0;
int totalLines=0;
int printedLines=0;
void do_more(FILE*);
int get_input(FILE*,int);
int totalLines(FILE*);
void main(int argc, char* argv[])
{
	struct winsize winSize;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize);
	PAGELEN = winSize.ws_row;
	if(argc==1)
	{
		do_more(stdin);
	}
	int i=0;
	while(++i<argc){
	FILE *fp;
	fp=fopen(argv[i],"r");
	if(fp == NULL)
	{
		perror("Can't Open File");
		exit(1);
	}
	do_more(fp);
	fclose(fp);
	}
}

void do_more(FILE* fp)
{
	int num_of_lines=0;
	int rv;
	printedLines=0;
	char buffer[LINELEN];
	int lines=totalLines(fp);
	
	FILE* fp_tty=fopen("/dev/tty","r");
	while(fgets(buffer,LINELEN,fp))
	{
		struct winsize winSize;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize);
		PAGELEN = winSize.ws_row;
		fputs(buffer,stdout);
		printedLines++;
		num_of_lines++;
		if(num_of_lines == PAGELEN)
		{
			int percent=(printedLines*100)/lines;
			rv=get_input(fp_tty,percent);
			if(rv ==0){
				printf("\033[1K \033[1G");
				break;
			}
			else if(rv ==1){
				num_of_lines-=PAGELEN;
				printf("\033[1K \033[1G");
			}
			else if(rv ==2){
				num_of_lines-=1;
				printf("\033[1A\033[2K\033[1G");
			}
			else if(rv==3){
				printf("\033[1K \033[1G");
				break;
			}
		}
	}
}

int get_input(FILE* cmdstream,int percent)
{	
	
	printf("\033[7m--more--(%d%%)\033[m",percent);
	int c;
	system("stty -icanon");
	c=getc(cmdstream);
	if(c=='q')
		return 0;
	if(c==' ')
		return 1;
	if(c=='\n')
		return 2;
	return 3;
	return 0;	
}

int totalLines(FILE* fp)
{
	char buff[LINELEN];
	totalLines=0;
	while(fgets(buff,LINELEN,fp))
	{
		totalLines++;
	}
	rewind(fp);
	return totalLines;
}









