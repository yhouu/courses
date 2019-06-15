#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <string.h>
#include <stdlib.h>
using namespace std;


bool range(int a, int b)
{
	if(a>=0&&a<15&&b>=0&&b<15)
		return true;
	else return false;
}




int main(int argc, char** argv)
{
	int x[8]={-2,-2,-1,-1,1,1,2,2}; // directions x, y can be moved to
	int y[8]={-1,1,-2,2,-2,2,-1,1};

	char delimeter[5]=" \0 EOF";
	int address[5]; //array with input values
	int i=0, sx=0,sy=0, dx=0, dy=0, mx=0,my=0, size=0;

	FILE* input =NULL;
	FILE* output=NULL;
	char* pch;
	
	queue<int> qx;
	queue<int> qy;


	input=open(argv[1], O_RDONLY);
	output=open(argv[2],O_RDWR);

	pch=strtok(argv[1],delimeter);

	while(pch) //토큰으로 받아서 address배열에 저장하기
	{
		address[i]=atoi(pch);
		i++;
		pch=strtok(NULL,delimeter);
	}
	
	size=address[0];
	sx=address[1]; //sx sy starting point
	sy=address[2];
	dx=address[3];//dx dy destination
	dy=address[4];
	
	qx.push(sx);
	qy.push(sy);

	while(!qx.empty())
	{
		for(int j=0;j<8;j++) //check which loc horse can move to from the current loc
		{
			mx=sx+x[j];
			my=sy+y[j];

			if(!range(mx,my)) //out-ranged
				continue;
			else if(range(mx,my)) //push x,y within the range
			{
				qx.push(mx);
				qy.push(my);
			}
		}
	
		


	}

	close(input);
	close(output);


}
