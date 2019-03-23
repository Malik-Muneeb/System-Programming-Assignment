#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdbool.h>

extern int errno;
void do_ls(char*);
void show_stat_info(char*,char*);
char* uidToUname(int uid);
char* gidToGname(int gid);
char* typeAndPermissions(int);
int cstring_cmp(const void *a, const void *b); //use for qsort()
void printFileNames(char *,char*);
void memoryAllocation();
int totalBlockSize=0;		//for total block size in a directory
char **directories;		//to save directories path name
int indexOfDirArr=0;		//index for directories array 
int numOfDir=0;			//record number of directories in given structure
int countNumOfDir(char*);	//return count of directories in a directory
bool isFirst=true;		//first time flag is true, allocate memory and then reallocate as new directories found
int loopIndex=-1;		//used for base condition in recursive call



int main(int argc, char* argv[])
{
	if(argc==1)
		do_ls(".");
	else
	{
		int i=1;
		while(i<argc)
		{
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
	printf("%s:\n\n", dir);
	int numOfLines=0;
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
				numOfLines++;
				i++;
			}
		}
	}

	qsort(names, numOfNames, sizeof(char *), cstring_cmp);	

	for(int i=0; i<numOfNames; i++)
	{
		char path[10000];
		strcpy(path,dir);
		strcat(path,"/");
		strcat(path,names[i]);
		numOfDir+=countNumOfDir(path);
	}	
	memoryAllocation();
	for(int i=0; i<numOfNames; i++)
	{
		char path[10000];
		strcpy(path,dir);
		strcat(path,"/");
		strcat(path,names[i]);
		show_stat_info(path,names[i]);
	}
	
	printf("\033[%dA\033[0GTotal: %d\033[G\033[%dB\n",numOfLines+1,totalBlockSize/2,numOfLines+1);
	numOfLines=0;
	totalBlockSize=0;
	if(loopIndex<numOfDir-1)
	{
		loopIndex++;
		char temp[10000];
		strcpy(temp,directories[loopIndex]);
		free(directories[loopIndex]);
		do_ls(temp);
		
	}
	
	//free(directories);
	closedir(dp);
}

int countNumOfDir(char* path)
{
	struct stat info;
	int rv=lstat(path,&info);
	if(rv==-1)
	{
		perror("stat failed");
		exit(1);
	}
	
	char *mode=typeAndPermissions(info.st_mode);
	if(mode[0]=='d')
		return 1;
	return 0;
}

void show_stat_info(char* path,char* fname)
{
	struct stat info;
	int rv=lstat(path,&info);
	if(rv==-1)
	{
		perror("stat failed");
		exit(1);
	}
	
	char *mode=typeAndPermissions(info.st_mode);
	totalBlockSize+=info.st_blocks;
	printf("%s\t",mode);
	printf("%ld\t",info.st_nlink);
	char *uName=uidToUname(info.st_uid);
	printf("%s\t",uName);
	char *gName=gidToGname(info.st_gid);
	printf("%s\t",gName);
	printf("%ld\t",info.st_size);
	struct tm tmStruct;
	localtime_r(&info.st_mtime, &tmStruct);
	char time_str[64];
	strftime (time_str, sizeof (time_str), "%b %e %H:%M",&tmStruct); 
	printf("%s\t",time_str);
	if(mode[0]=='d')
	{
		directories[indexOfDirArr] = (char*)malloc(sizeof(char) * (strlen(path) + 1 ) );
		strcpy(directories[indexOfDirArr],path);
		indexOfDirArr++;
		
	}
	printFileNames(mode,fname);
}

char* uidToUname(int uid)
{
	errno=0;
	struct passwd * pwd=getpwuid(uid);
	if(pwd==NULL)
	{
		if(errno==0)
			return "Unknown user";
		else
			printf("getpwuid failed");
	}
	else	
		return pwd->pw_name;
}

char* gidToGname(int gid)
{
	errno=0;
	struct group * grp=getgrgid(gid);
	if(grp==NULL)
	{
		if(errno==0)
			return "Unknown group";
		else
			printf("getprgid failed");
	}
	else	
		return grp->gr_name;
}

char* typeAndPermissions(int mode)
{
	char *str=malloc(sizeof(char)*10);
	strcpy(str,"----------");
	if((mode & 0170000) == 0010000)
		str[0]='p';
	else if((mode & 0170000) == 0020000)
		str[0]='c';
	else if((mode & 0170000) == 0040000)
		str[0]='d';
	else if((mode & 0170000) == 0060000)
		str[0]='b';
	else if((mode & 0170000) == 0100000)
		str[0]='-';
	else if((mode & 0170000) == 0120000)
		str[0]='l';
	else if((mode & 0170000) == 0140000)
		str[0]='s';
	else 
		str[0]='~';		//unknown
	if((mode & 0000400) == 0000400)
		str[1]='r';
	if((mode & 0000200) == 0000200)
		str[2]='w';
	if((mode & 0000100) == 0000100)
		str[3]='x';
	if((mode & 0000040) == 0000040)
		str[4]='r';
	if((mode & 0000020) == 0000020)
		str[5]='w';
	if((mode & 0000010) == 0000010)
		str[6]='x';
	if((mode & 0000004) == 0000004)
		str[7]='r';
	if((mode & 0000002) == 0000002)
		str[8]='w';
	if((mode & 0000001) == 0000001)
		str[9]='x';
	if((mode & 0004000) == 0004000)
		str[3]='s';
	if((mode & 0002000) == 0002000)
		str[6]='s';
	if((mode & 0001000) == 0001000)
		str[9]='t';	
	//printf("%s\n",str);
	return str;
}

int cstring_cmp(const void *a, const void *b) 
{ 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
	/* doesn't understand how its work */ 
} 


void printFileNames(char *mode,char *fname)
{
	if(mode[0]=='d')
		printf("\033[34m\033[1m%s\033[39m\033[m\n",fname);
	else if(mode[0]=='l')
		printf("\033[36m\033[1m%s\033[39m\033[m\n",fname);
	else if(mode[0]=='p')
		printf("\033[33m\033[40m%s\033[39m\033[49m\n",fname);
	else if(mode[0]=='c')
		printf("\033[33m\033[1m\033[40m%s\033[39m\033[m\033[49m\n",fname);
	else if(mode[0]=='b')
		printf("\033[33m\033[1m\033[40m%s\033[39m\033[m\033[49m\n",fname);
	else if(mode[0]=='s')
		printf("\033[35m\033[1m%s\033[39m\033[m\n",fname);
	else if(strstr(fname,".out"))
		printf("\033[32m\033[1m%s\033[39m\033[m\n",fname);
	else if(strstr(fname,".tar") || strstr(fname,".tar.gz") )
		printf("\033[31m\033[1m%s\033[39m\033[m\n",fname);
	else
		printf("%s\n",fname);
	return;
}

void memoryAllocation()
{
	if(isFirst)
	{
		directories = (char**)malloc(sizeof(char**)*numOfDir);
		isFirst=false;
	}
	else
	{
		//printf("num of dir: %d",numOfDir);
		char** tempDir;
		tempDir = (char**)realloc(directories,sizeof(char**)*numOfDir);
		if(tempDir==NULL)
			perror("realloc fialed\n");
		else
			directories=tempDir;
	}	

}
