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

extern int errno;
void do_ls(char*);
void show_stat_info(char*,char*);
char* uidToUname(int uid);
char* gidToGname(int gid);
char* typeAndPermissions(int);
int totalBlockSize=0;
int main(int argc, char* argv[])
{
	if(argc==1)
	{
		do_ls(".");
	}
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
	printf("\n"); //for print total blocks at end
	int numOfLines=0;
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
			{
				char path[1000];
				strcpy(path,dir);
				strcat(path,"/");
				strcat(path,entry->d_name);
				numOfLines++;
				show_stat_info(path, entry->d_name);
			}					
		}
	}
	printf("\033[%dA\033[0GTotal: %d\033[G\033[%dB",numOfLines+1,totalBlockSize/2,numOfLines+1);
	closedir(dp);
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
	printf("%s\n",fname);
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
