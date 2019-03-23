#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

	
int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		printf("Enter exactly one argument (a directory name)\n");
		exit(1);
	}
	struct stat buf;
	if(lstat(argv[1], &buf)<0)
	{
		fprintf(stderr,"Error in stat\n");
		exit(1);
	}
	if((buf.st_mode & 0170000) == 0010000)
		printf("%s is Named Pipe",argv[1]);
	else if((buf.st_mode & 0170000) == 0020000)
		printf("%s is Character Special",argv[1]);
	else if((buf.st_mode & 0170000) == 0040000)
		printf("%s is Directory",argv[1]);
	else if((buf.st_mode & 0170000) == 0060000)
		printf("%s is Block Special",argv[1]);
	else if((buf.st_mode & 0170000) == 0100000)
		printf("%s is Regular",argv[1]);
	else if((buf.st_mode & 0170000) == 0120000)
		printf("%s is Soft Link",argv[1]);
	else if((buf.st_mode & 0170000) == 0140000)
		printf("%s is Socket",argv[1]);
	else 
		printf("Unknown");
	return 0;
}

