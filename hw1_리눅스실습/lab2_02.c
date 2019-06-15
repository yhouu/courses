#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//cat함수 
int main(int argc, char** argv)
{
 	int f1, f2; 
	char buffer[512];
	
	f1=open(argv[1],O_RDWR,0755);
	f2=open(argv[2],O_RDWR,0755);

	read(f1, buffer, sizeof(buf));
	lseek(f2,-1,SEEK_END);
	write(f2, buffer, sizeof(buf));

	close(f1);
	close(f2);	
}
