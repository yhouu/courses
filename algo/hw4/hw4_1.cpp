#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
using namespace std;

int main(int argc, char*argv[])
{
	int set=0, op=0,x=0,y=0;
	int **table=NULL;
	int **check=NULL;
	cout.setf(ios::fixed);
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);

	fin>>set;
	fin>>op;

	table=new int*[op]; //create input  table
	for(int i=0;i<op;i++)
		table[i]=new int[3];

	check=new int*[set]; //create checking table
	for(int i=0;i<set;i++)
		check[i]=new int[set];



	for(int i=0;i<op;i++)	{ 
		for(int j=0;j<3;j++)
			fin>>table[i][j]; //copy input into table
	}

	for(int i=0;i<set;i++){
		for(int j=0;j<set;j++)
			check[i][j]=0; //initialize array
	}

	for(int i=0;i<op;i++){
		x=table[i][1];
		y=table[i][2];
		if(table[i][0]==0)	{
			check[x][y]=1; // '1' = two vertices are connected
			check[y][x]=1;
		}

		if(table[i][0]==1)	{
			if(check[x][y]==1) //connected
				fout<<'Y'<<endl; //print Y
			else if(check[x][y]==0)//unconnected
				fout<<'N'<<endl; //print N
		}
	}
	
	for(int i=0;i<op;i++){
		for(int j=0;j<3;j++)	fout<<table[i][j];
		fout<<endl;		
	}

	fout<<"------"<<endl;

	for(int i=0;i<set;i++){
		for(int j=0;j<set;j++) fout<<check[i][j];
		fout<<endl;}

	for(int i=0;i<op;i++) delete[] table[i];
	delete[] table;
	for(int i=0;i<set;i++) delete[] check[i];
	delete[] check;

	return 0;	

}
