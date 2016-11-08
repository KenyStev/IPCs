#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

void* thread_fuction(void*);

int main(int argc, char const *argv[])
{
	pthread_t thread;

	int number = rand();
	printf("Rand num: %d\n", number);

	pthread_create(&thread, NULL, thread_fuction, (void*) &number);

	for (int i = 0; i < 100; ++i)
	{
		printf("Main waiting: %d\n", i);
		usleep(1);	
	}

	pthread_join(thread, NULL);
	printf("Thread ended!\n");
	return 0;
}

void* thread_fuction(void *arg){
	int i = 0;

	int num = *(int*)arg;
	printf("Thread received: %d\n", num);

	for (i = 0; i < 110; ++i)
	{
		printf("Thread: %d\n", i);
		usleep(2);
	}

	return NULL;
}