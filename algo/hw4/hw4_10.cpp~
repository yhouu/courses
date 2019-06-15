#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
using namespace std;

int checking(int x, int y, vector<int> *v, int *visited);


int main(int argc, char*argv[])
{
	int set=0, op=0,x=0,y=0,cnnct=-1;
	int **table=NULL;
	int *visited=NULL;

	cout.setf(ios::fixed);
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);

	fin>>set;
	fin>>op;

	table=new int*[op];
	for(int i=0;i<op;i++)
		table[i]=new int[3];
		
	for(int i=0;i<op;i++){
		for(int j=0;j<3;j++)
			fin>>table[i][j];
	}

	visited=new int[set+1];
	for(int i=0;i<set+1;i++)
		visited[i]=0;

	vector<int> v[set+1];

	for(int i=0;i<op;i++){	
		x=table[i][1];//set
		y=table[i][2];//connected vertex

		if(table[i][0]==0)	{ //union
			v[table[i][1]].push_back(table[i][2]);
			v[table[i][2]].push_back(table[i][1]);
		}		

		else if(table[i][0]==1){ //checking
			cnnct=checking(x, y, v,visited);
			cout<<cnnct<<endl;
			if(cnnct==1) fout<<'Y'<<endl;
			else if(cnnct==0) fout<<'N'<<endl;
		}
	}


	for(int i=0;i<op;i++) delete[] table[i];
	delete[] table;


	return 0;
}

int checking(int x, int y, vector<int> *v,int *visited)
{
	int result=0, sz=0;
	
	sz=v[x].size();
	cout<<"checking "<<x<<sz<<endl;
	while(result==0)
	{
		for(int j=0;j<sz;j++){
			if(v[x][j]==y) 
			{
				result=1;
				return result;
			}
		}
		for(int j=0;j<sz;j++)
		{	if(visited[v[x][j]]!=0) continue;

			checking(v[x][j],y,v,visited);
		}
	}
	
	return result;
}
