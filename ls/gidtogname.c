#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <grp.h>

extern int errno;
int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		printf("You must enter one cmd arg (gid)\n");
		exit(1);
	}
	int gid=atoi(argv[1]);
	errno=0;
	struct group * grp=getgrgid(gid);
	if(grp==NULL)
	{
		if(errno==0)
			printf("Record not found in /etc/group file");
		else
			printf("getprgid failed");
	}
	else	
		printf("Name of group is: %s\n",grp->gr_name);
	return 0;
}

