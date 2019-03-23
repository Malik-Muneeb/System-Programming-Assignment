#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int errno;
int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		printf("You must enter one cmd arg (Number of seconds)\n");
		exit(1);
	}
	long secs=atoi(argv[1]);
	printf("Date for %ld seconds since epoch is: %s\n",secs,ctime(&secs));
	return 0;
}

