#include <stdio.h>
#include <string.h>
#include <my_string.h>

int main()
{
	char str1[20] = "Hello";
	char str2[20] = "world!";
	int num = 3500, n=6;

	printf("<my_atoi> str1: %zu\n", my_atoi(str1));
	printf("<my_atol> str1: %zu\n", my_atol(str1));
	printf("<int2str> str1: %zu\n", int2str(str1, num));
	printf("<strcpy> dest: %s\n", strcpy(str1, str2));
	printf("<strcat> dest: %s\n", strcat(str1, str2));


	return 0;
}
