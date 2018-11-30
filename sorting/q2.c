#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

///////////////////////////DECLARATIONS/////////////////////////////////

//variables
int totbooth;//no of booths
int votcount,evmcount;//total voters and total evms

//structures
typedef struct Evm
{
    int id;//pk
    int slots;//total slots(variable)
    int boothid;//fk
    int check;
    pthread_t evmid;
}evm;

typedef struct Booth
{
    int id;//pk
    int no_evms;
    int no_voters;
    int voted;//how many voted
    pthread_t boothid;
    pthread_mutex_t mutex_booth;
    pthread_cond_t cond1;
    pthread_cond_t cond2;
}booth;

typedef struct Voter
{
    int id;//pk
    int booth;//fk
    int evm;//fk
    pthread_t voterid;
}voter;

//functions
booth* boothset(int,int,int);//initialize booth
void voteset(int,int);//initialize voter
void evmset(int,int);//initialize evm

//thread functions
void*booth_thread(void*);//booth thread
void*evm_thread(void*);//evm thread
void*voter_thread(void*);//voter thread

//arrays
# define N 10000//max no of voters and evms
booth**bootharr;//declaring pointer to array of pointers
evm evmarr[N];//array of evms
voter voterarr[N];//array of voters
int status[N];//voter status(at which stage of voting)
int check[N];//voter check

///////////////////////////////////////////////////////////////////////
///////////////////////////////MAIN////////////////////////////////////

int main()
{
    int i,j,k;//for counting
    votcount=0;
    evmcount=0;
    int a,b;//storing intermediate info

    printf("Enter number of Booths-> ");
    scanf("%d",&totbooth);

    bootharr=(booth**)malloc(sizeof(booth**)*totbooth);
    printf("Enter no_of_voters and no_of_evms for each Booth\n");

    //setting voter initial cond
    for(i=0;i<N;i++)
    {
        voterarr[i].booth=-1;//to avoid default 0 value
        voterarr[i].evm=-1;//to avoid defult 0 value
        voterarr[i].id=i;//pk
    }
    //setting evm initial cond
    for(i=0;i<N;i++)
    {
        evmarr[i].id=i;//pk
        evmarr[i].boothid=-1;//to avoid default 0 value
    }

    //boothset
    for(i=0;i<totbooth;i++)
    {
        scanf("%d%d",&a,&b);
        bootharr[i]=boothset(i,a,b);

        //evmset
        for(j=0;j<b;j++)
        {
            evmset(evmcount,i);
            evmcount++;//acts as id
        }

        //voterset
        for(k=0;k<a;k++)
        {
            voteset(votcount,i);
            votcount++;//acts as id
        }
    }

    printf("\nSTART OF ELECTIONS\n");
    sleep(2);
    //create thread for each booth
    for(i=0;i<totbooth;i++)
        pthread_create(&(bootharr[i]->boothid),NULL,booth_thread,(void*)i);//(void*)i to pass int argument

    //join thread for every booth
    for(i=0;i<totbooth;i++)
        pthread_join(bootharr[i]->boothid,&i);

    printf("END OF ELECTIONS\n\n");
    sleep(2);

    return 0;
}

//////////////////////////////////////////////////////////////////////
///////////////////////INITIALIZERS///////////////////////////////////

booth* boothset(int i,int a,int b)
{
    booth*aa=(booth*)malloc(sizeof(booth));//creating pointer
    
    //booth info
    aa->id=i;
    aa->no_evms=b;
    aa->no_voters=a;
    aa->voted=0;
    pthread_mutex_init(&aa->mutex_booth, NULL);
    pthread_cond_init(&aa->cond1, NULL);
    pthread_cond_init(&aa->cond2,NULL);
    return aa;
}

void evmset(int j,int id)
{
    //evm info
    evmarr[j].boothid=id;
    evmarr[j].check=0;
}

void voteset(int votcount,int i)
{
    //voter info
    voterarr[votcount].booth=i;
    status[votcount]=0;
}

///////////////////////////////////////////////////////////////////
/////////////////BOOTH THREAD//////////////////////////////////////

void*booth_thread(void*args)
{
    int booth=((int)args);//booth id
    int i,j;//counting variables


    //create evm threads
    for(i=0;i<bootharr[booth]->no_evms;)
    {
        for(j=0;j<N;j++)
        {
            //searching for evms belonging to this booth
            if(evmarr[j].boothid==booth)
            {   
                i++;
                pthread_create(&evmarr[j].evmid,NULL,evm_thread,(void*)j);
            }
        }
    }

    //creating voter threads
    for(i=0;i<bootharr[booth]->no_voters;)
    {
        for(j=0;j<N;j++)
        {
            //searching for voters belonging to this booth
            if(voterarr[j].booth==booth)
            {
                i++;
                pthread_create(&voterarr[j].voterid,NULL,voter_thread,(void*)j);
            }
        }
    }

    //joining evm threads
    for(i=0;i<bootharr[booth]->no_evms;)
    {
        for(j=0;j<N;j++)
        {
            if(evmarr[j].boothid==booth)
            {
                i++;
                pthread_join(evmarr[j].evmid,NULL);
            }
        }
    }

    //joining voter threads
    for(i=0;i<bootharr[booth]->no_voters;)
    {
        for(j=0;j<N;j++)
        {
            if(voterarr[j].booth==booth)
            {
                i++;
                pthread_join(voterarr[j].voterid,NULL);
            }
        }
    }

    printf("Voters at Booth %d are done with voting\n",bootharr[booth]->id);
    sleep(3);
}
////////////////////////////////////////////////////////////////////////////
////////////////////RVM THREAD//////////////////////////////////////////////

void*evm_thread(void*args)
{
    int evmid=((int)args);//evm id
    int x=evmarr[evmid].boothid;//booth id
    booth* booth=bootharr[x];//booth=booth(pointer)

    //conditions and mutexes
    pthread_mutex_t *mutex = &(booth->mutex_booth);
    pthread_cond_t *cond1 = &(booth->cond1);
    pthread_cond_t *cond2 = &(booth->cond2);

    while(1)
    {
        int i,k;//usage discussed later

        //checking if all voters have voted
        pthread_mutex_lock(mutex);
        if(booth->voted == booth->no_voters)
            {
                pthread_mutex_unlock(mutex);
                break;
            }
        pthread_mutex_unlock(mutex);

        k=0;//to how many evm allocated
        pthread_mutex_lock(mutex);
        evmarr[evmid].check=0;
        evmarr[evmid].slots=rand()%10;//setting random slots to the evm
        printf("EVM %d at Booth %d is free with slots = %d\n",evmid,booth->id,evmarr[evmid].slots);
        sleep(3);
        pthread_mutex_unlock(mutex);
    

        while(1)
        {
            if(k>evmarr[evmid].slots)//if max limit reached
                break;
            else
            {
                pthread_mutex_lock(mutex);

                //picking random voter
                i=rand()%votcount;//voter index

                if(status[i]==1 && voterarr[i].booth==x)//if waiting for evm and belongs to this booth
                {
                    voterarr[i].evm=evmid;
                    status[i]=2;
                    printf("Voter %d at Booth %d got allocated EVM %d\n",i,booth->id,evmarr[evmid].id);
                    sleep(3);
                    booth->voted++;
                    k++;
                }

                if(booth->voted==booth->no_voters)
                {
                    pthread_mutex_unlock(mutex);
                    break;
                }

                pthread_mutex_unlock(mutex);
            }
        }

        if(k==0)
            break;

        pthread_mutex_lock(mutex);

        evmarr[evmid].slots=k;
        printf("EVM %d at Booth %d is moving for voting stage\n",evmarr[evmid].id,booth->id);
        evmarr[evmid].check=1;
        pthread_cond_broadcast(cond1);
        while(evmarr[evmid].slots)
            pthread_cond_wait(cond2,mutex);//waiting for voters to cast their vote
        pthread_mutex_unlock(mutex);
    
    }
    printf("EVM %d at Booth %d has Finished voting\n",evmarr[evmid].id,booth->id);
    sleep(3);
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////VOTER THREAD/////////////////////////////////////////////

void*voter_thread(void*args)
{
    int voter=((int)args);//voter id
    int x=voterarr[voter].booth;//index of booth
    booth* booth=bootharr[x];//booth=booth

    pthread_mutex_t *mutex = &(booth->mutex_booth);
    pthread_cond_t *cond1 = &(booth->cond1);
    pthread_cond_t *cond2 = &(booth->cond2);

    printf("Voter %d of Booth %d is waiting for EVM\n",voterarr[voter].id,booth->id);
    sleep(3);
    pthread_mutex_lock(mutex);
    status[voter]=1;
    while(status[voter]==1)
        pthread_cond_wait(cond1,mutex);//waiting for being allocated

    pthread_mutex_unlock(mutex);

    int y=voterarr[voter].evm;//index of evm

    pthread_mutex_lock(mutex);

    while(evmarr[y].check==0)//waits till evm ready for voting stage
        pthread_cond_wait(cond1,mutex);//waiting to move to voting stage

    evmarr[y].slots--;

    pthread_cond_broadcast(cond2);
    printf("Voter %d has casted his vote\n",voterarr[voter].id);
    sleep(3);
    pthread_mutex_unlock(mutex);
}

/////////////////////////////////////////////////////////////////////////////////////