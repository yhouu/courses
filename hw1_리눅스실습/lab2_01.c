#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char**argv)
{
	int f1, f2, c;
	char buf[512];
	
	//copy
	//argv1이 source, argv2가 destination
	//자동으로
	//printf("%d %s %s\n", argc, argv[1], argv[2]);

	f1=open(argv[1],O_RDWR);
	f2=open(argv[2],O_RDWR);


	read(f1,buffer,sizeof(buf));
	write(f2, buffer, sizeof(buf));

	
	close(f1);
	close(f2);
}
