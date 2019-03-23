#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>

extern int errno;
int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		printf("You must enter one cmd arg (uid)\n");
		exit(1);
	}
	int uid=atoi(argv[1]);
	errno=0;
	struct passwd * pwd=getpwuid(uid);
	if(pwd==NULL)
	{
		if(errno==0)
			printf("Record not found in passwd file");
		else
			printf("getpwuid failed");
	}
	else	
		printf("Name of user is: %s\n",pwd->pw_name);
	return 0;
}

