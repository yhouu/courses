#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "my_string.h"

int main()
{
		char str1[20] = "s";
		char str2[20] = "asbbscca";
		char in2st[20];
				int num = 3500, n=6;

		//	printf("<my_atoi> str1: %zu\n", my_atoi(str1));
		//	printf("<my_atol> str1: %zu\n", my_atol(str1));
		//	printf("<int2str> str1: %zu\n", int2str(str1, num));
			printf("<strcpy> dest: %s\n", strcpy(str1, str2));
			printf("<strcat> dest: %s\n", strcat(str1, str2));
			printf("<strstr> haystack: %s\n", strstr(str1,str2));
			printf("<strchr> str: %s\n", strchr(str2, 'r'));
			printf("<strncmp> value: %d\n", strncmp(str1,str2,4));
			printf("<strcmp> value: %d\n", strcmp(str1,str2));
			printf("<strpbrk> str1: %s\n", strpbrk(str1, str2));
			sprintf(in2st,"%d",num);
			printf("<int2str> in2st: %s\n", in2st);
			printf("<strtok> str: %s\n", strtok(str1, str2));


		return 0;
}
