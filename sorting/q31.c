#include<stdio.h>
int arr[1000000],aux[1000000];
int mergesort(int,int);
int merge(int,int,int);
int main()
{
	int n,i;
	printf("enter number of inputs\n");
	scanf("%d",&n);
	for (i=0;i<n;i++)
		scanf("%d",&arr[i]);

	mergesort(0,n-1);
	
    for(i=0;i<n;i++)
		printf("%d ",arr[i]);
    printf("\n");
	
    return 0;
}

int mergesort(int low,int high)
{
	if(low!=high)
	{
		int mid=(low+high)/2;
		mergesort(low,mid);
		mergesort(mid+1,high);
		merge(low,mid,high);
	}
	return 0;
}

int merge(int low,int mid,int high)
{
	int i=low,j=mid+1,t=0;
	while(i<=mid && j<=high)
	{
		if(arr[i]<=arr[j])
		{
			aux[t]=arr[i];
			i++;
			t++;
		}
		else
		{
			aux[t]=arr[j];
			j++;
			t++;
		}
	}
	while(i>mid && j<=high)
	{
		aux[t]=arr[j];
		j++;
		t++;
	}
	while(j>high && i<=mid)
	{
		aux[t]=arr[i];
		i++;
		t++;
	}
	for(i=0;i<t;i++)
		arr[low+i]=aux[i];
	return 0;
}
