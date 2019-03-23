#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LINELEN 512
#define PAGELEN 20

void do_more(FILE*);
int get_input(FILE*);

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
	char buffer[LINELEN];
	FILE* fp_tty=fopen("/dev/tty","r");
	while(fgets(buffer,LINELEN,fp))
	{
		fputs(buffer,stdout);
		num_of_lines++;
		if(num_of_lines == PAGELEN)
		{
			rv=get_input(fp_tty);
			if(rv ==0)
				break;
			else if(rv ==1){
				num_of_lines-=PAGELEN;
				printf("\033[1A\033[2K\033[1G");
			}
			else if(rv ==2){
				num_of_lines-=1;
				printf("\033[1A\033[2K\033[1G");
			}
			else if(rv==3)
				break;
		}
	}
}

int get_input(FILE* cmdstream)
{
	printf("\033[7m--more--(%%)\033[m");
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










