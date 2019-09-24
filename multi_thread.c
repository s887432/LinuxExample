#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct __thread_obj__
{
	int id;
	int delay;
}thread_obj, *pthread_obj;

void thread_proc(void *param)
{
	int count = 0;
	pthread_obj obj = (pthread_obj)param;
	
	while(1)
	{
		printf("I am thread %d, %d\n", obj->id, count++);
		sleep(obj->delay);
	}
}

thread_obj threadA;
thread_obj threadB;

int main(int argc, char *argv[])
{
    pthread_t idA, idB;
    int i,ret;
	
	threadA.id = 100;
	threadA.delay = 1;
	
	threadB.id = 200;
	threadB.delay = 3;
	
    ret = pthread_create(&idA, NULL, (void *) thread_proc, &threadA);
    if(ret!=0)
	{
        printf ("Create pthread error!n");
        return (1);
    }

    
	ret = pthread_create(&idB, NULL, (void *) thread_proc, &threadB);
    if(ret!=0)
	{
        printf ("Create pthread error!n");
        return (1);
    }

    pthread_join(idA, NULL);
    pthread_join(idB, NULL);
	
	return (0);
}
