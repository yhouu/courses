#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
using namespace std;

int main(int argc, char*argv[])
{
	int column=0, row=0,vertex=0, edges=0;
	int **table=null;
	cout.setf(ios::fixed);
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);

	fin>>vertex;
	fin>>edges;

	table=new int*[vertex]
	for(int i=0;i<vertex;i++)
		table=new int[edges];

	for(int i=0;i<edges;i++){ //copy input
		for(int j=0;j<3;j++)
			fin>>table[i][j];
	}

	int vector<pair<int,int>> v[vertex+1];
	
	for(int i=0;i<edges;i++)
		v[table[i][0]].push_back(make_pair(table[i][1],table[i][2]));



	

	




}
