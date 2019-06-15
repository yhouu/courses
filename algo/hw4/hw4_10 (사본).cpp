#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
using namespace std;

int checking(int x, int y, vector<int> *v);


int main(int argc, char*argv[])
{
	int set=0, op=0,x=0,y=0;
	int **table=NULL;

	cout.setf(ios::fixed);
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);

	fin>>set;
	fin>>op;

	table=new int*[op];
	for(int i=0;i<op;i++)
		table[i]=new int[3];
		
	
	vector<int> v[set];

	for(int i=0;i<op;i++){	
		x=table[i][1];//set
		y=table[i][2];//connected vertex

		if(table[i][0]==0)	{
			v[x].push_back(y);
			v[y].push_back(x);
		}		

		else if(table[i][0]==1){	
			checking(x, y, v);
		}
	}


	for(int i=0;i<op;i++) delete[] table[i];
	delete[] table;

	return 0;
}

int checking(int x, int y, vector<int> *v)
{
	int result=0, sz=0;
	sz=v[x].size();
	for(int j=0;j<sz;j++){
		if(v[x][j]==y){
			result=1;
			return result;
		}

		else{
			checking(v[x][j],y,v);
		}
	}
	
	return result;
}
