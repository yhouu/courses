//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (2018 Fall)
//
// Skeleton code for PA #0
// September 12, 2018
//
// Somm Kim, Sooyun Lee
// Embedded Software Laboratory
// Sungkyunkwan University
//
// Student Name	: lee young suk(이영숙)
// Student ID	: 2014314433
//
//-----------------------------------------------------------

#include "my_string.h"
#include <stdlib.h>

#define isspace(ch) ((ch) == ' ' || (ch) == '\t' || (ch) == '\n' || \
					 (ch) == '\v' || (ch) == '\f' || (ch) == '\r')

/* Conversion of string to numeric format */
int my_atoi(const char *str) //문자열 str을 정수형 int로 변환하여 출력
{
	//int ret = 0;
	//int sign = 0; // 부호가 나왔다.
	//int minus = 0;	// minus다 아니다
	//int base = 1;
	//int valid = 1;
	//int idx = 0;
	//

	//// isspace가 True인 한 기다린다. null문자 아닌 한.(while)
	//char cur = str[idx];
	//while (isspace(str[idx]) && str[idx] != '\0') {
	//	idx++;
	//}
	//if (str[idx] == '\0') valid = 0; //계속 스페이스있는거... 

	//if (valid == 1) {
	//	// -, +, 0-9가 나오는 한 한칸씩 가면서 숫자를 ret에 누적
	//	while (str[idx] != '\0') {
	//		// 부호도 아니고 숫자도 아니고 안되는것
	//		// invalid

	//		// 부호이고 처음 등장
	//		// 
	//	}

	//	// 유효하지 않은 문자면 invalid 후 return
	//	// minus나 plus가 나오면 sign=1;
	//	// minus면 minus를 1로
	//	// plus면 그냥 패스
	//	// 숫자 --> ret += 숫자 * base
	//	// base *= 10;
	//}

	//return ret;
}

long my_atol(const char *str) //문자열 str을 정수형 long으로 변환하여 출력 
{
}

/* Conversion of numeric format to string */
char *int2str(char *dest, int num) {
	char buf[12];

	int cnt = -1, temp=1;
	while (num >= 1)
	{
		temp =num / 10;
		cnt++; //num의 자릿수를 알려줄 놈!
	}

	for (int i = 0;i < cnt;i++)
	{
		
	}

	return dest;
}

/* String manipulation */
char *strcpy(char *dest, const char *src) {
	int i=0;
	while (src[i] != '\0') //src에서 dest로 복사해줌, src이 문자열 끝을 나타내는 \0 문자 만나기 전까지 for문 돌림 
	{
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0'; //dest 끝에 종료 문자 넣어주기

	return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
	int i = 0;
	while (i < n && src[i] != '\0')//길이가 n보다 적고 src가 n보다 작지 않아 종료문자 없을때 복사 
	{
		dest[i] = src[i];
		i++;
	}
	
	while (i < n)//n보다 길이가 짧아서 n바이트 복사가 불가능할 때는 그 뒤에 쭉 널바이트 넣음!
	{
		dest[i] = '\0';
		i++;
	}

	return dest;
}

char *strcat(char *dest, const char *src) {
	int i = 0;

	while (dest[i] != '\0') //원래 dest 스트링 끝 인덱스 찾기
		i++;

	for (int j = 0;src[j] != '\0';j++)
	{
		dest[i] = src[j]; //끝인덱스부터 src값 대입하기
		i++;
	}

	dest[i] = '\0';

	return dest; 
}
	


char *strncat(char *dest, const char *src, size_t n) {
	int j = 0, i = 0, dsize = 0, ssize=0;

	for (j = 0;dest[j] != '\0';j++) //dest 길이 파악
		dsize++;


	for (j = 0;j < n&&src[j] != '\0';j++) //길이가 n보다 적고 src가 n보다 작지 않아 종료문자 없을때 복사 
	{
		dest[dsize] = src[j];
		dsize++;
	}
	
	dest[dsize] = '\0';
	return dest;

}

char *strdup(const char *str) {
	int size = 0; 

	for (int i = 0;str[i] != '\0';i++)
		size++; //str 사이즈 찾기 

	char *newbie = (char*)malloc(sizeof(char)*size); //위에서 찾은 str 사이즈만큼 새로운 new에 동적할당
	
	for (int i = 0;str[i] != '\0';i++)
		newbie[i] = str[i]; //new에 str 복사

	newbie[size] = '\0';

	return newbie;
	
}

/* String examination */
size_t strlen(const char *str) {
	int i = 0;
	while (str[i] != '\0') //길이 구하기
		i++;

	return i; //널문자 만났을때 인덱스 출력시키기
}

int strcmp(const char *str1, const char *str2) {
	/*-1 s2가 큼
		0 같음
		1 s1이 큼*/
	int i = 0, ret=0;
	while (str1[i] != '\0'||str2[i] != '\0')
	{
		if (str1[i] > str2[i]) //str1이 클때
			return 1;
		else if (str1[i] < str2[i]) //str2가 클때 
			return -1;

		i++;
	}

	if (str1[i] == '\0'&&str2[i] == '\0') //두 스트링 모두 null이면 같은 문자
		return 0;
	else if (str1[i] == '\0') //str2가 더 긴 문자
		return -1;
	else //str1이 더 긴 문자
		return 1;
}

int strncmp(const char *str1, const char *str2, size_t n) {
	/*-1 s2가 큼
	0 같음
	1 s1이 큼*/

	int i = 0;
	while ((str1[i] != '\0' || str2[i] != '\0')&&(i<n)) //i<n을 추가해주고싶오용..
	{
		if (str1[i] > str2[i]) //str1이 클때
			return 1;
		else if (str1[i] < str2[i]) //str2가 클때 
			return -1;

		i++;
	}

	if (i < n) //str중에 한쪽이 더 짧음
	{
		if (str2[i] == '\0')//str1이 더 긴 문자
			return 1;
		else if (str1[i] == '\0') //str2가 더 긴 문자
			return -1;
	}
	else //str 비교 길이 같음, n까지 채워진것!!!
		return 0;
	
}

char *strchr(const char *str, int c) {
	int i = 0;

	if (c == '\0')
	{
		while (str[i] != '\0')
			i++;

		return  str + i;
	}
		
	else
	{
			while (str[i] != '\0')
			{
				if (str[i] == 'c')
					break; //while 빠져나가기
				else
					i++;
			}

			if (str[i] != '\0')
				return NULL; //c가 없을때 \0 에 대한 포인터 출력함
			else
				return str + i;
	}
}

char *strrchr(const char *str, int c) {
	int i = 0, last=-1; //last -1로 한 이유는 인덱스 0에서 c가 나올수도있으니까

	while (str[i] != '\0')
	{
		if (str[i] == 'c')
		{
			last = i; //last에 c 출현한 인덱스 저장
			i++;
		}	
		else
			i++;
	}

	if (last = -1)
		return NULL; //null 반환
	else
		return str+last; //last 위치 포인터 반환
}

char *strpbrk(const char *str1, const char *str2) {
	//str1에서 str2중에 어떤 문자든 str1에 나타나면 그 첫번째 값 v포인터 반환

	int s1 = 0, s2 = 0,i=0,j=0;

	while (str1[s1] != '\0') //str1 길이구하기
		s1++;

	while (str1[s2] != '\0') //str2 길이구하기
		s2++;

	for (i = 0;i < s1;i++) //str1이랑 str2비교하기 ->str1 앞에서부터
	{
		for (j = 0;j < s2;j++) //->str2전체와 비교한다
		{
			if (str1[i] == str2[j]) //str1이랑  str2가 같으면 str2 서치 break
				break;
		}

		if (j < s2) //str1이랑  str2가 같으면 str1 서치 break
			break;
	}

	if (i >= s1) //str1에 str2의 문자가 없어서 끝까지 다 비교한 경우
		return NULL;
	else
		return str1 + i;
}

char *strstr(const char *haystack, const char *needle) {
	//haystack에 부분 문자열 needle이 있는 첫번째 포인터 주소를 반환함

	int a=0,h = 0, n = 0,i=0,j=0, cnt=0, len=0;
	
	while (haystack[h] != '\0') //hay 길이구하기
		h++;
	while (needle[n] != '\0') //need 길이구하기
		n++;

	char *index = (char*)malloc(sizeof(char)*h); //hay길이만큼의 new동적할당

		for (j = 0;j < h;j++) //haystack전체에서 needle 시작 될 수 있는 위치들 저장
		{
			if (haystack[j] == needle[0])
			{
				index[cnt] = j;
				cnt++;
			}
		}
		index[cnt] = '\0';
		j = 0;

	while (index[len] != '\0') //index배열 길이구하기
		len++;

	cnt = 0;
	if (len == 0) return NULL;
	
	else {
		for (a = 0;a < len;a++)
		{
			i = index[a];
			for (j = 0;j < n;j++) //첫문자 시작위치부터 needle 길이만큼만 돌기
			{
				if (haystack[i + j] == '\0'&& needle[j] != '\0')
					return NULL;
				else if (haystack[i + j] == needle[j])
					cnt++;
				else if (haystack[i + j] != needle[j])
					break;
			}
			if (cnt >= n) //needle 끝까지 비교가 가능했고 비교 대상들이 다 같았다는것.
				break;
		}


		return haystack + index[a];
	}
}

char *strtok(char *str, const char *delim) {
	static char *tok = NULL;


}

char *strtok_r(char *str, const char *delim, char **save_ptr) {
}

/* Character array manipulation */
void *memcpy(void *dest, const void *src, size_t n) {
	(char*)dest;
	(char*)src;

	for(int i=0;i<n;i++)
	{
		*((char*)dest+i) = *((char*)src+i);
		//(char*)src[i]
		//src[i]
	}
	return dest;
}

void *memset(void *str, int c, size_t n) {
	for (int i = 0;i < n;i++)
	{
		*((char*)str+i)=c;
	}
	
	return str;
	
}
