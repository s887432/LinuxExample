#include <stdio.h>
#include <pthread.h>

typedef struct __thread_obj__
{
	int id;
	int delay;
}thread_obj, *pthread_obj;

void thread_proc(void *param)
{
	pthread_obj obj = (pthread_obj);
	
	while(1)
	{
		printf("I am thread %d\n", obj->id);
		sleep(obj->delay);
	}
}

thread_obj threadA;
thread_obj threadB;

int main(void)
{
    pthread_t idA, idB;
    int i,ret;
	
	threadA.id = 100;
	threadA.delay = 1;
	
	threadB.id = 200;
	threadB.delay = 3;
	
    ret = pthread_create(&idA, NULL, (void *) thread_proc, $threadA);
    if(ret!=0)
	{
        printf ("Create pthread error!n");
        exit (1);
    }
    pthread_join(idA, NULL);
    
	ret = pthread_create(&idB, NULL, (void *) thread_proc, &threadB);
    if(ret!=0)
	{
        printf ("Create pthread error!n");
        exit (1);
    }
    pthread_join(idB, NULL);
	
	return (0);
}