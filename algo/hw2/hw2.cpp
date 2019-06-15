#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>
#include <queue>
#include <bits/stdc++.h>
#include <sys/time.h>
#include <sys/resource.h>

#define SWAP(X,Y,T) (T=X, X=Y,Y=T)
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define MAX(X,Y) ((X)>(Y)?(X):(Y))

using namespace std;


void bubble(int* list, int j);  
void insertion(int* list, int n); 

//MERGE SORT  NOT YET
void mergesort(int* list,int first, int last,int length);
void merge(int* list, int left, int mid, int right, int length);

//HEAP SORT 
void heap(int* list,int length);
void insertheap(int* list, int length, int pri);


void quick(int* list,int left,int right); 

//RADIX SORT 
void radix(int* list,int length);

void stacksize();

int main(int argc, char*argv[])
{
    char * type = argv[1];
	ifstream fin(argv[2]);
	ofstream fout(argv[3]);

	// input --> array
	cout.setf(ios::fixed);
	int length=0;
	int i=0;
	char c= *type;
	cout.setf(ios::fixed);
	clock_t start, end;
	double  exe_time;

	stacksize();
	fin>>length; //read	length from the file

	int v[length];//creat an array 

	for(i=0;i<length;i++)
		fin>>v[i];
	
	v[i]=EOF;
	

	// type --> sort
	//B=BUBBLE, I=INSERTION, M=MERGE, H=HEAP, Q=QUICK, R=RADIX
	switch(c)
	{
		case 'B':	
			start=clock();
			bubble(v, length);	
			end=clock();
			cout<<"Bubblesort for ";
			break;
		case 'I':
			start=clock();
			insertion(v, length);
			end=clock();
			cout<<"Insertionsort for ";
			break;
		case 'M':
			start=clock();
			mergesort(v,0,length-1,length);
			end=clock();
			cout<<"Mergesort for ";
			break;
		case 'H':
			start=clock();
			heap(v,length);
			end=clock();
			cout<<"Heapsort for ";
			break;
		case 'Q':
			start=clock();
			quick(v,0,length-1);
			end=clock();
			cout<<"Quicksort for ";
			break;
		case 'R':
			start=clock();
			radix(v,length);
			end=clock();
			cout<<"Radixsort for ";
			break;
		default:
			break;
	}

	exe_time=(double)(end-start)/CLOCKS_PER_SEC;
	cout.precision(6);
	cout<<length<<" integers took " << exe_time <<" seconds."<<endl;


	// output --> output file
	fout<<length<<endl;
	for(int i=0;i<length;i++)
		fout<<v[i]<<" ";

	fout<<endl;

	return 0;
}



void bubble(int* list, int n)
{
	int length=n, temp;
	
	for(int i=length-1;i>0;i--) //starting from the back
	{
		for(int j=0;j<i;j++) //compare previous numbers
		{
			if(list[j]>list[j+1]) //swap if list[j] is bigger than list[j+1]
				SWAP(list[j],list[j+1],temp);
		}
	}
}


void insertion(int* list, int n)
{
	int length=n,i,j,key,temp;

	for(i=1;i<n;i++) //list[0] is considered as a sorted array, sorting starts from list[1]
	{
		key=list[i]; //list[i] is going to be compared with(0~ i-1) members
		for(j=i-1;j>=0;j--)
		{
			if(list[j]>key) // if list[i-1] is bigger than list[i]
				list[j+1]=list[j];//swap~~

			else break;
		}
		list[j+1]=key;
	}
}


void mergesort(int* list,int first, int last,int length)
{	
	int mid=(last+first)/2;

	if(first<last) 
	{
		mergesort(list, first, mid, length);
		mergesort(list, mid+1, last, length);
		merge(list, first, mid, last,length);
	}

}


void merge(int* list, int first, int mid, int last,int length)
{
	int second=mid+1, f=first, r=last, k=first,m,n;
	int temp[length]={0};

	//mid is located between left and right
	while(f<=mid && second<=last)
	{
		temp[k++] = (list[f]>list[second])? list[second++]:list[f++];
	/*	if(list[first]<=list[second]) //if an element is smaller than mid, add it to the first part of temp
			temp[i++]=list[first++];
		else 
			temp[i++]=list[second++]; //if an element is greater than mid, add it to the second part of temp*/
	}

	m=(mid<f)?second:f; //start
	n=(mid<f)?last:mid; //end
	
	for(;m<=n;m++)
		temp[k++]=list[m];
/*	
	if(mid<first) //first part of array is done
	{
		for(int j=second;j<=last;j++) //add the rest of part to temp
			temp[f++]=list[j];
	}

	else 
	{
		for(int j=first;j<=mid;j++) //add the rest of first part to temp
			temp[f++]=list[j];
	}
*/
	//copy temp to original array
	for(int m=first;m<=last;m++) 
		list[m]=temp[m];
		
	
}

void heap(int* list,int length)
{
	int temp=0;
	for(int i=length/2-1;i>=0;i--)
		insertheap(list,length,i);

	for(int i=length-1;i>=0;i--)
	{
		SWAP(list[0],list[i],temp);
		insertheap(list,i,0);
	}

}

void insertheap(int*list, int length, int pri)
{
	int lcidx=(pri+1)*2-1;
	int rcidx=(pri+1)*2;
	int largest=pri;
	int temp=0;

	if(lcidx<length && list[lcidx]>list[largest])
		largest = lcidx;

	if(rcidx<length &&list[rcidx]>list[largest])
		largest=rcidx;

	if(largest!=pri)
	{
		SWAP(list[pri],list[largest],temp);
		insertheap(list, length,largest);
	}
}

void quick(int* list,int left,int right)
{
	int L=left, R=right, temp, length=right+1;
	int pivot= list[(L+R)/2]; //pivot is the middle point of left and right

	
	while(L<=R) //L and R are index(not value)
	{
		while(list[L]<pivot) L++; 
		while(list[R]>pivot) R--;

		if(L<=R)
		{
			if(L!=R) //L is less than R -> Swap
				SWAP(list[L], list[R], temp);
			
			L++; R--; //move the index
						
		}		
	}
		
	if(left<R)
		quick(list, left, R);
	if(L<right)
		quick(list, L, right);

}


void radix(int* list,int length)
{	
	int i=0, a=0, exp=1;
	int *arr=(int*)malloc(sizeof(int)*length+1);

	
	for(i=0;i<length;i++)
		a=MAX(a,list[i]); //finding MAX
	
	while(a/exp>0)
	{
		int bucket[10]={-1};

		for(i=0;i<length;i++)
			bucket[list[i]/exp%10]++;

		for(i=1;i<10;i++)
			bucket[i]+=bucket[i-1];

		for(i=length-1;i>=0;i--)
			arr[--bucket[list[i]/exp%10]]=list[i];

		for(i=0;i<length;i++)
			list[i]=arr[i];

		exp *=10;
	}
	
	free(arr);
}

void stacksize()
{
	const rlim_t newStackSize=64*1024*1024;
	struct rlimit resLimit;
	int result;

	result=getrlimit(RLIMIT_STACK, &resLimit);
	if(result==0)
	{
		if(resLimit.rlim_cur<newStackSize)
		{
			resLimit.rlim_cur = newStackSize;
			if(setrlimit(RLIMIT_STACK, &resLimit))
			{
				printf("Stack Allocation Error. \n");
				exit(0);
			}
		}
	}
}
