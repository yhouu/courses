#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	int f1, f2;
	char buffer[512];

	f1=open("sentence.txt", O_RDWR, 0755);
	f2=open("word.txt", O_RDWR, 0755);


	fgets(f1);
/*
	A=The
	The=A
	dog-cat
	fgets=*/

	strtok(

	close(f1);
	close(f2);
}

