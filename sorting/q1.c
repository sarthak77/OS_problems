#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

void*org(void*arg);//thread for organizer
void*ent(void*arg);//thread for while loop for entering
void*enter(void*arg);//thread for people entering academy

int n;//total matches
int rp;//remaining players
int rr;//remaining referees
int pw;//player entered till now
int rw;//referee entered till now
int pm;//players meeted organizer
int rm;//referees meeted organizer
int match;//matches held till now

pthread_mutex_t player;//mutex for player
pthread_mutex_t referee;//mutex for referee
pthread_mutex_t organ;//mutex for organizer

int main()
{
    //initializing mutexes
    pthread_mutex_init(&player, NULL);
    pthread_mutex_init(&referee, NULL);
    pthread_mutex_init(&organ, NULL);

    printf("Input n::\n");
    scanf("%d",&n);
    
    //initializing variables
    match=0;
    pw=0;
    rw=0;
    rp=2*n;
    rr=n;
    pm=0;
    rm=0;

    //creating threads
    pthread_t organizer;
    pthread_create(&organizer, NULL, org, NULL);
    pthread_t entering;
    pthread_create(&entering, NULL, ent, NULL);

    //joining threads
	pthread_join(entering, NULL); 
	pthread_join(organizer, NULL); 
    
}

void*ent(void*arg)
{
    //checking if more people to come
    while(rp>0 || rr>0)
    {
        pthread_t person;
        pthread_create(&person,NULL,enter,NULL);
    	pthread_join(person, NULL); 
    }
}

void*enter(void*arg)
{
        //flag for player
        int flag=0;

        //if player => flag=1
        if(rp*10/(rp+rr)>=5)
            flag=1;

        if(flag)//player
        {
    		pthread_mutex_lock(&player);
            pw++;
            rp--;
            int t=rand()%3;
            sleep(t);
            printf("Player%d entered academy\n",pw);
    		pthread_mutex_unlock(&player);
        }
        else//referee
        {
    		pthread_mutex_lock(&referee);
            rw++;
            rr--;
            printf("Referee%d entered academy\n",rw);
    		pthread_mutex_unlock(&referee);
        }
}

void*org(void*arg)
{
    while(pm!=2*n || rm!=n)//run until all meet
    {
        pthread_mutex_lock(&organ);

        if(pw>pm)//check if new player entered
        {
            pthread_mutex_lock(&player);
            pm++;
            printf("Player%d met the organizer\n",pm);
            pthread_mutex_unlock(&player);
        }
        if(rw>rm)//check if new referee entered
        {
            pthread_mutex_lock(&referee);
            rm++;
            printf("Referee%d met the organizer\n",rm);
            pthread_mutex_unlock(&referee);
        }

        //no need of thread as players and referee can do in any order which can be done using rand()
        if(rm>=match+1 && pm>=2*match+2)//if grup of 3 formed
        {
            printf("Player%d entered the court\n",2*match+1);
            printf("Player%d started warming up for the game\n",2*match+1);
            sleep(1);

            printf("Player%d entered the court\n",2*match+2);
            printf("Player%d started warming up for the game\n",2*match+2);
            sleep(1);

            printf("Referee%d entered the court\n",match+1);
            printf("Referee%d started adjusting equipment for the game\n",match+1);
            sleep(.5);

            printf("Referee%d started the game\n",match+1);
            match++;
        }

        pthread_mutex_unlock(&organ);  
    }
}