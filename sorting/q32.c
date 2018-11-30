#include<stdio.h>
#include<sys/types.h> 
#include<sys/wait.h> 
#include<sys/ipc.h> 
#include<sys/shm.h> 
#include<stdlib.h> 
#include<unistd.h> 

key_t key = IPC_PRIVATE;

int *arr,aux[1000000];
int mergesort(int,int);
int merge(int,int,int);
int selectionsort(int,int);

int main()
{
    int shmid;//shared memory id
	int n,i;
	printf("enter number of inputs\n");
	scanf("%d",&n);

    shmid = shmget(key, sizeof(int) *n, IPC_CREAT| 0666);
	if(shmid == -1) {
		perror("Shmget failed");
		exit(1);
	}

    arr = shmat(shmid, 0, 0);//attaching memory
	if(arr == (void *)-1) {
		perror("Shmat failed");
		exit(1);
	}

	for (i=0;i<n;i++)
		scanf("%d",&arr[i]);

	mergesort(0,n-1);
	
    for(i=0;i<n;i++)
		printf("%d ",arr[i]);
    printf("\n");
	
	if (shmdt(arr) == -1)//detatch memory 
	{ 
		perror("shmdt"); 
		_exit(1); 
	} 

	if (shmctl(shmid, IPC_RMID, NULL) == -1)//delete memory 
	{ 
		perror("shmctl"); 
		_exit(1); 
	} 

    return 0;
}

int mergesort(int low,int high)
{
    int len=(high-low+1);
	int mid=(low+high)/2;

    if(len<5)//selectionsort for small arrays
    {
        selectionsort(low,len);
        return 0;
    }

    __pid_t lpid,rpid;

    lpid=fork();//left child
    if(lpid<0)
    {
        perror("left child not created\n");
        _exit(-1);
    }
    else if(lpid==0)
    {
        mergesort(low,mid);
        _exit(0);
    }
    else
    {
        rpid=fork();//right child
        if(rpid<0)
        {
            perror("right child not created\n");
            _exit(-1);
        }
        else if(rpid==0)
        {
            mergesort(mid+1,high);
            _exit(0);
        }
    }

    int status;
    waitpid(lpid, &status, 0); 
	waitpid(rpid, &status, 0); 

	merge(low,mid,high);
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

int selectionsort(int low,int len)
{
    int i,j,mi;
    for(i=low;i<low+len;i++)
    {
        mi=i;//min index
        for(j=i+1;j<low+len;j++)
        {
            if(arr[j]<arr[mi])
                mi=j;
        }

        int temp=arr[mi];
		arr[mi]=arr[i];
		arr[i]=temp;
    }
    return 0;
}
