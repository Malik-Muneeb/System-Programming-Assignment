#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LINELEN 512
#define PAGELEN 20

int Tlines=0;
int printedLines=0;
void do_more(FILE*);
int get_input(FILE*,int);
int TLines(FILE*);
void main(int argc, char* argv[])
{
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
	int lines=TLines(fp);
	
	FILE* fp_tty=fopen("/dev/tty","r");
	while(fgets(buffer,LINELEN,fp))
	{
		fputs(buffer,stdout);
		printedLines++;
		num_of_lines++;
		if(num_of_lines == PAGELEN)
		{
			int percent=(printedLines*100)/lines;
			rv=get_input(fp_tty,percent);
			if(rv ==0){
				break;
			}
			else if(rv ==1){
				num_of_lines-=PAGELEN;
				printf("\033[1A\033[2K\033[1G");
			}
			else if(rv ==2){
				num_of_lines-=1;
				printf("\033[1A\033[2K\033[1G");
			}
			else if(rv==3){
				//printf("\033[1A\033[2K\033[1G");
				break;
			}
		}
	}
}

int get_input(FILE* cmdstream,int percent)
{	
	
	printf("\033[7m--more--(%d%%)\033[m",percent);
	int c;
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

int TLines(FILE* fp)
{
	char buff[LINELEN];
	Tlines=0;
	while(fgets(buff,LINELEN,fp))
	{
		Tlines++;
	}
	rewind(fp);
	return Tlines;
}









