#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
typedef struct my_pthread_mutex_t;
typedef struct pthread_retval
{
    int flag;
    int id;
    void** waitingForValuePtr;
    struct pthread_retval *next;
} pthread_retval;

typedef struct my_pthread_t
{
    //void** waitingForValuePtr;
    pthread_retval *returnList;
    void* valuePtr;
    int id;
    int activeFlag;
    struct my_pthread_t *next;
    struct my_pthread_t *prev;
    ucontext_t context;
    long int lastTimeFinished;
    long int startTime;
    long int totalRunTime;
    int waitingOnID;
    struct my_pthread_mutex_t *ownedMutex;
} my_pthread_t;

typedef struct priorityQueue
{
    int numNodes;
    my_pthread_t *front;
    my_pthread_t *rear;
} priorityQueue;
void checker();
void createThreadNode(my_pthread_t **node);

void enqueue(my_pthread_t * p, int num);

my_pthread_t * dequeue(int num);

void initQueue();

long int timeStamp();

void scheduler();

//void alarm(int signum);

int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg);

void my_pthread_yield();

void pthread_exit(void *value_ptr);

typedef struct my_pthread_mutex_t{
    int mutexID;
    volatile int flag;
    volatile int block;
    int owner_thread; //thread that owns the mutex
    priorityQueue * wait;
    struct my_pthread_mutex_t *next;
} my_pthread_mutex_t;

int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);

int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);


