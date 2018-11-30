#include<stdio.h>
#include<pthread.h>

typedef struct
{
    int low;
    int high;
} thread_data_t;

int arr[1000000],aux[1000000];

void *mergesort(void*);
int merge(int,int,int);
int selectionsort(int,int);

pthread_mutex_t mutex;

int main()
{
	int n,i;
	printf("enter number of inputs\n");
	scanf("%d",&n);
	for (i=0;i<n;i++)
		scanf("%d",&arr[i]);

    thread_data_t a;
    a.low=0;
    a.high=n-1;

	pthread_t thread;
    pthread_create(&thread, NULL, mergesort, &a);
	pthread_join(thread, NULL);
	
    for(i=0;i<n;i++)
		printf("%d ",arr[i]);
    printf("\n");
	
    return 0;
}

void *mergesort(void*arg)
{
	thread_data_t *a = (thread_data_t *) arg;
    int l=a->low;
    int r=a->high;
    int len=r-l+1;
    int mid=(l+r)/2;

    if(l>=r)
        pthread_exit(0);

    if(len<5)
    {
        pthread_mutex_lock(&mutex);
        selectionsort(l,len);
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
    }
    else if(l<r)
    {
        pthread_t thread0,thread1;
        thread_data_t b,c;

        b.low=l;
        b.high=mid;
        c.low=mid+1;
        c.high=r;

        pthread_create(&thread0, NULL, mergesort, &b);
        pthread_create(&thread1, NULL, mergesort, &c);

   	    pthread_join(thread0, NULL); 
       	pthread_join(thread1, NULL); 

        pthread_mutex_lock(&mutex);
        merge(l,mid,r);
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
    }
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
	
    return 0 ;
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
