#include <stdio.h>
#include <stdlib.h>

#define LINELEN 512

void do_more(FILE*);

void main(int argc, char* argv[])
{
	if(argc==1)
	{
		printf("This is the help page");
		exit(0);
	}
	FILE *fp;
	fp=fopen(argv[1],"r");
	if(fp == NULL)
	{
		perror("Can't Open File");
		exit(1);
	}
	do_more(fp);
	fclose(fp);
}

void do_more(FILE* fp)
{
	char buffer[LINELEN];
	while(fgets(buffer,LINELEN,fp))
	{
		fputs(buffer,stdout);
	}
}
