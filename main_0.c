#include "my_pthread_t.h"

my_pthread_mutex_t * lock_a;
my_pthread_mutex_t *lock_b;
static int counter;
static int NTHREADS; // set thread amt
int f = 1;
void* doSomeThing(void *arg)
{

    printf("about to go into the mutex lock FROM main\n");
    my_pthread_mutex_lock(lock_a);
    my_pthread_mutex_lock(lock_b);
    counter += 1;
    unsigned long i = 0;
    printf("\n Job %d has lock\n", counter);
    for(i=0; i<(0xFFFFFFFF);i++);
    printf("\n Job %d finished\n", counter);
    printf("about to go into mutex UNLOCK FROM main\n");
    my_pthread_mutex_unlock(lock_b);
    my_pthread_mutex_unlock(lock_a);
    f++;
    //printf("*******IN MAIN F = %d\n", f);
    //my_pthread_exit(&f);
    //return f;
}

int main(int argc, char ** argv)
{   
	// NTHREADS = first arg
    
	
	if(argc == 2)
		NTHREADS = atoi(argv[1]);
	else
		NTHREADS = 3;
    
    printf("Starting Testing\n");
    printf("Allocating space for the thread array\n");
    int *ptr[3];
    my_pthread_t *thread_list = (my_pthread_t *)malloc(NTHREADS * sizeof(my_pthread_t));
    
    lock_a = malloc(sizeof(my_pthread_mutex_t)); 
    lock_b = malloc(sizeof(my_pthread_mutex_t));
    printf("Initializing the Mutex\n");
    //my_pthread_mutex_t * mutex0 = malloc(sizeof(my_pthread_mutex_t));
    my_pthread_mutex_init(lock_a,NULL);
    my_pthread_mutex_init(lock_b,NULL);
    
    
    printf("Initializing thread\n");
    time_t t;
    long int i;
    long int base = 100;
    long int random[NTHREADS];
    long int random_sec[NTHREADS];
    //start = getTime();
    
    srand((unsigned) time(&t));
    
    for (i = 0; i < NTHREADS; i++) {
        random[i] = rand() % 1000 * base;
        printf("Random Number %li\n", random[i]);
    }   
    
    thread_list = malloc(NTHREADS * sizeof(my_pthread_t));
    
    for (i = 0; i < NTHREADS; i++) {
        printf("i = %d\n",i);
	if (my_pthread_create(&thread_list[i], NULL, doSomeThing, (void *)random[i]) != 0) {
            printf("Error Creating Thread %li\n", i);
        }
    }
    int x;
    for(x=0; x<NTHREADS; x++){
        printf("in the for loop in main haha\n");
	my_pthread_join(thread_list[x], (void**)&ptr[x]);
    }
    checker();
    printf("about to go into for loop\n");
    /*for(x=0;x<NTHREADS;x++){
	printf("*****%p******\n",*(ptr[x]));
    	//printf("*****%p******\n", ptr[1]);
	//printf("*****%p******\n", ptr[2]);
    }*/
    //my_pthread_yield();
    my_pthread_mutex_destroy(lock_a);
    my_pthread_mutex_destroy(lock_b);
    free(lock_a);
    free(lock_b);

    printf("HEYEYEEYEYEYEYEYEYEYE!! finished with everything peace\n");
    return 0;
}

/*********************************************************************************************************/
/*
void * CountEven();
void * CountOdd();
my_pthread_mutex_t *count_mutex;
static int count = 0;
#define COUNT_DONE 10

void * CountEven(){
    for(;;){
        my_pthread_mutex_lock(count_mutex);
        if(count%2 ==0){
            printf("%d is even \n", count);
        }
        count++;
        if(count>= COUNT_DONE)
        {
            my_pthread_mutex_unlock(count_mutex);
            return NULL;
        }

       my_pthread_mutex_unlock(count_mutex);
    }
}

void * CountOdd(){
    printf("hi");
    for(;;){

        my_pthread_mutex_lock(count_mutex);
        if(count%2 != 0){
            printf("%d is odd\n", count);
        }
        count++;
        if(count>= COUNT_DONE)
        {
            my_pthread_mutex_unlock(count_mutex);
            return NULL;
        }

        my_pthread_mutex_unlock(count_mutex);
    }
}


int main(){

    count_mutex = malloc(sizeof(my_pthread_mutex_t));
    printf("initializing mutex\n");
    my_pthread_mutex_init(count_mutex,NULL);

    printf("MAIN:initializing thread structs\n");

    //my_pthread_t thread1, thread2;

    my_pthread_t thread1, thread2;

    int ret1, ret2, ret3;

    printf("creating pthreads\n");

   // ret3 =my_pthread_mutex_destroy(count_mutex);

   // ret1 = my_pthread_create(&thread1, NULL, &CountEven, NULL);
    //ret2 = my_pthread_create(&thread2, NULL, &CountOdd, NULL);

    ret1 = my_pthread_create(&thread1, NULL, &CountEven, NULL);
    ret2 = my_pthread_create(&thread2, NULL, &CountOdd, NULL);


    printf("after pthread_create(), joining pthreads...\n");

    my_pthread_join(thread1, NULL);
    my_pthread_join(thread2, NULL);

    //my_pthread_join(thread1, NULL);
    //my_pthread_join(thread2, NULL);

    printf("pthreads joined\n");
    my_pthread_mutex_destroy(count_mutex);

    free(count_mutex);

    exit(0);


}*/