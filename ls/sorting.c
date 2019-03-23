#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;
int cstring_cmp(const void *a, const void *b);
void print_cstring_array(char **array, int len);
void do_ls(char*);
int main(int argc, char* argv[])
{
	if(argc==1)
		do_ls(".");
	else
	{
		int i=1;
		while(i<argc)
		{
			printf("Directory listing of %s:\n", argv[i]);
			do_ls(argv[i]);
			i++;
		}
	}
	return 0;
}

void do_ls(char* dir)
{
	struct dirent * entry;
	DIR* dp=opendir(dir);
	if(dp==NULL)
	{
		fprintf(stderr,"cannot open directory %s\n",dir);
		return;
	}
	errno=0;
	int numOfNames=0;
	while((entry=readdir(dp))!=NULL)
	{
		if(entry==NULL && errno!=0)
		{
			perror("readdir failed");
			exit(errno);
		}
		else
		{
			if(entry->d_name[0]!='.')
				numOfNames++;	//just count number of files
		}
	}

	char **names;
	names = (char**)malloc(sizeof(char**)*numOfNames);
	int i=0;
	dp=opendir(dir);
	while((entry=readdir(dp))!=NULL)
	{
		if(entry==NULL && errno!=0)
		{
			perror("readdir failed 1");
			exit(errno);
		}
		else
		{
			if(entry->d_name[0]!='.')
			{
				names[i] = (char*)malloc(sizeof(char) * (strlen(entry->d_name) + 1 ) );
				strcpy(names[i], entry->d_name);
				i++;
			}
		}
	}

	qsort(names, numOfNames, sizeof(char *), cstring_cmp);
	print_cstring_array(names, numOfNames);
	closedir(dp);
}

void print_cstring_array(char **array, int len) 
{ 
    int i;
 
    for(i=0; i<len; i++) 
        printf("%s \t", array[i]);
 
    putchar('\n');
} 

int cstring_cmp(const void *a, const void *b) 
{ 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
	/* strcmp functions works exactly as expected from
	comparison function */ 
} 
