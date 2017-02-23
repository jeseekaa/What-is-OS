#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <ucontext.h>
#include "my_pthread_t.h"




/*Generic queue functions*/
void create_queue(queue * q){

    q->head = NULL;
    q->tail = NULL;
    q->size =0;
}

void enqueue(queue * q, my_pthread_t * thread_node){

    if(q->size ==0){ //empty queue
        q->head = thread_node;
        q->tail = thread_node;
        q->size++;
    }
    else{ // FIFO, add to tail
        q->tail->nxt_pthread = thread_node;
        q->tail = thread_node;
        q->size++;

    }
}

my_pthread_t * dequeue(queue * q){
    
    //check empty
    if(q->size ==0){
        printf("Empty Queue\n");
        return NULL;
    }

    //FIFO, dequeue from head, create a temp to return the value
    my_pthread_t * temp;
    if(q->size ==1){
        temp = q->head;
        q->head = NULL;
        q->tail = NULL;
    }
    else{
        temp = q->head;
        q->head = q->head->nxt_pthread;
    }

    q->size--;

    return temp;

}

my_pthread_t * peek(queue * q){
    return q->head;
}

int isEmpty(queue *q){
    //0 true, 1 false
    if(q->size ==0){
        return 0;
    }else{
        return 1;
    }
}

/*helper function: getTime*/
long int getTime(){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    return 1000000 * current_time.tv_sec + current_time.tv_usec;
}

/* Scheduling functions*/

void schedule_handler(){
    struct itimerval tick;
    ucontext_t sched_ctx;
    
    //clear the timer
    tick.it_value.tv_sec = 0;
    tick.it_value.tv_usec = 0;
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &tick, NULL);

    //perform aging
    if(check_flag++ >= CHECK_FRQ){
        // printf("Start scaling up threads...\n");
        int i;
        check_flag = 0;
        long int current_time = getTime();
        for (i = 1; i < NLEVELS; i++) {
            if (sched->MLPqueue[i].head != NULL) {
                my_pthread_t* temp = sched->MLPqueue[i].head;
                my_pthread_t* parent = NULL;
                while(temp != NULL){
                    if(current_time - temp->last_exe_tt >= AGE_THRSHLD){
                        //delete from current queue
                        if(parent == NULL){
                            sched->MLPqueue[i].head = temp->nxt_pthread;
                        }else{
                            parent->nxt_pthread = temp->nxt_pthread;
                        }
                        //put the thread to the highest queue
                        add_mypthread_sched(temp, 0);
                    }else{
                        parent = temp;
                    }
                    temp = temp->nxt_pthread;
                }
            }
        }
    }
    
    //schelduling
    my_pthread_t* tmp = sched->thread_curr;
    if(tmp != NULL){
        int old_priority = tmp->priority;
        tmp->time_ran += TIME_QUANT;
        if(tmp->time_ran >= sched->priority_list[old_priority] || tmp->thread_state == YIELD || tmp->thread_state == TERMINATED 
            || tmp->thread_state == WAITING){
            if (tmp->thread_state == TERMINATED){
                // free(tmp);
            }else if(tmp->thread_state == WAITING){
                //do nothing, the thread is already in the wait queue of the mutex
            }else if(tmp->thread_state == YIELD){
                //put the thread back into the original queue
                add_mypthread_sched(tmp, tmp->priority);
            }else{
                //put the thread back into the queue with the lower priority
                int new_priority = (tmp->priority+1) > (NLEVELS-1) ? (NLEVELS-1) : (tmp->priority+1);
                add_mypthread_sched(tmp, new_priority);
            }
            //pick another thread out and run
            if((sched->thread_curr = sched_chooseThread()) != NULL){
                sched->thread_curr->thread_state = RUNNING;
            } 
        }
    }else{
        //pick another thread out and run
        if((sched->thread_curr = sched_chooseThread()) != NULL){
            sched->thread_curr->thread_state = RUNNING;
        } 
    }

    //set timer
    tick.it_value.tv_sec = 0;
    tick.it_value.tv_usec = 50000;
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &tick, NULL);

    //if(tmp != NULL){
        //getcontext(&sched_ctx);
        //tmp->ucontext_t context_ptr = sched_ctx;
    //}
    
    if(sched->thread_curr != NULL){
        if(sched->thread_curr->first_exe_tt == 0){
            sched->thread_curr->first_exe_tt = getTime();
        }
        sched->thread_curr->last_exe_tt = getTime();
        if( tmp != NULL)
            swapcontext(&(tmp->context_ptr), & (sched->thread_curr->context_ptr) );
        else
            swapcontext(&sched_ctx, &(sched->thread_curr->context_ptr));
    }
    return;
}

void init_schedule(){

    int i, j, k;
    
    sched = malloc(sizeof(scheduler));
    sched->MLPqueue = malloc(NLEVELS * sizeof(queue));
    sched->wait = malloc(NLOCKS  * sizeof(queue));
    sched->thread_main = (my_pthread_t *) calloc(1, sizeof(my_pthread_t));

    for (i = 0; i < NLEVELS; i++) {
        create_queue((sched->MLPqueue) + i);
    }
    for (j = 0; j < NLOCKS; j++) {
        create_queue((sched->wait) + j);
    }
    for (k = 0; k < NLEVELS; k++) {  // This is a temporary placeholder
        sched->priority_list[k] = TIME_QUANT * (k+1);    // for storing scheduling times, could be logrithm
    }
    
    sched->num_sched = 0;

    sched->thread_main->pthread_id = 0;
    sched->thread_main->thread_state = NEW;
    sched->thread_main->nxt_pthread = sched->thread_main;
    sched->thread_curr = NULL;

    signal(SIGALRM, schedule_handler);
    schedule_handler();
}

void add_mypthread_sched(my_pthread_t *thread_node, int priority){
    if (priority < 0 || priority >= NLEVELS) {
        printf("Error: Priority not in range.\n");
    } else {
        printf("Adding thread %d to level %d\n", thread_node->pthread_id, priority); //testing
        thread_node->thread_state = READY;
        printf("change thread_state to READy\n");
        thread_node->priority = priority; 
        printf("priority set\n");
        thread_node->time_ran = 0; // reset the running time of the thread
        printf("running time set\n");
        enqueue(&(sched->MLPqueue[priority]), thread_node);
        printf("enqueued\n");
        sched->num_sched++;
    }
}

my_pthread_t * sched_chooseThread(){
    int i;

    for (i = 0; i < NLEVELS; i++) {
        if (sched->MLPqueue[i].head != NULL) {
            my_pthread_t * chosen = dequeue(&(sched->MLPqueue[i])); 
            // printf("Found a thread to schedule in level %d, thread id: %d\n", i, chosen->thr_id);
            sched->num_sched--;
            return chosen;
        }
    }

    printf("Nothing to schedule.\n");


    // Print Everything 
    end = getTime();
    printf("Start time: %ld\n", start);
    printf("End time: %ld\n", end);
    printf("Total time: %ld\n", end - start);

    
    for (i = 0; i < NTHREADS; i++) {
        printf("Response time of %d is : %ld\n", i, ( (&thread_list[i])->first_exe_tt  - (&thread_list[i])->start_tt) );
    }

    return NULL;
}


void run_mypthread(my_pthread_t * thread_node, void *(*f)(void *), void * arg){
    thread_node->thread_state = RUNNING;
    sched->thread_curr = thread_node;
    thread_node->return_value = f(arg);

    if (thread_node->thread_state != TERMINATED) {
        thread_node->thread_state = TERMINATED;
    } 

    if(sched->thread_curr != NULL){
        sched->thread_curr->end_tt=getTime();
    }
    schedule_handler();
}

/*pthread library*/ 

int my_pthread_create( my_pthread_t * thread, const pthread_attr_t * attr, void *(*function)(void*), void * arg){
/*Creates a pthread that executes function. Attributes are ignored.*/

    if(getcontext(&thread->context_ptr)==-1){
        printf("ERROR: getcontext \n");
        return -1;
    }

    thread->context_ptr.uc_stack.ss_sp = malloc(STACK_SIZE); //func_stack
    thread->context_ptr.uc_stack.ss_size = STACK_SIZE;

    thread->pthread_id++;
    thread->start_tt = getTime();
    thread->first_exe_tt = 0;
    printf("Allocating the stack\n");
    makecontext(&(thread->context_ptr), (void *)run_mypthread, 3, thread, function, arg);
    printf("Made Context\n");
    add_mypthread_sched(thread, 0);
    printf("Added Thread to the Scheduler.\n");
    return 0;
}

void my_pthread_yield(){
/*Explicit call to the my_pthread_t scheduler requesting that the current context be swapped out and another be scheduled.*/
    sched->thread_curr->thread_state = YIELD;

    schedule_handler();
}

void my_pthread_exit(void *value_ptr){
/*Explicit call to the my_pthread_t library to end the pthread that called it. If the value_ptr isn't NULL, any return value from the thread will be saved.*/
    if (sched->thread_curr->thread_state == TERMINATED) {
        printf("This thread has already exited.\n");
    }
    sched->thread_curr->thread_state = TERMINATED;
    sched->thread_curr->return_value = value_ptr;
    sched->thread_curr->end_tt=getTime();
    // call the scheduler
    schedule_handler();

}

int my_pthread_join(my_pthread_t * thread, void **value_ptr){
/*Call to the my_pthread_t library ensuring that the calling thread will not execute until the one it references exits. If value_ptr is not null, the return value of the exiting thread will be passed back.*/


    while(thread->thread_state != TERMINATED){
        my_pthread_yield();
    }

    thread->return_value = value_ptr;

}



/*mutex functions: Jessica 

int functions return 0 upon success*/

int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr){
/*Initializes a my_pthread_mutex_t created by the calling thread. Attributes are ignored.*/
    int init = 1;

    if(mutex == NULL){
        return EINVAL;
    }

    mutex->flag =0;
    mutex->block =0;
    mutex->wait = malloc(sizeof(queue));
    create_queue(mutex->wait);

    return init=0;
}

int my_pthread_mutex_lock(my_pthread_mutex_t *mutex){
/*Locks a given mutex, other threads attempting to access this mutex will not run until it is unlocked.*/
    int locking = 1;

    //spinlocking because he said something like that in class idk i was half asleep anf my head hurts

    while(__sync_lock_test_and_set(&(mutex->flag), 1)==1){
        sched ->thread_curr->thread_state = WAITING; //set state of thread to be WAITING on mutex
        enqueue(mutex->wait, sched->thread_curr);

        schedule_handler();
    }

    return locking=0;
}

int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex){
/*Unlocks a given mutex.*/
    int unlock =1;

    my_pthread_t * target_thread;
    if(mutex->wait->head != NULL){
        target_thread = dequeue(mutex->wait);
        //mutex is available 

        add_mypthread_sched(target_thread, target_thread->priority);
    }

    mutex->flag =0;

    return unlock;

}


int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex){
/*Destroys a given mutex. Mutex should be unlocked before doing so.*/
    int destroy =1;

    if(mutex ==NULL){
        return EINVAL;
    }

    if(mutex->flag != 0){
        return EBUSY;
    }

    return destroy=0;
}

/********************************** testing below *********************************/

my_pthread_mutex_t lock;
int counter;

void* doSomeThing(void *arg)
{
    my_pthread_mutex_lock(&lock);

    unsigned long i = 0;
    counter += 1;
    printf("\n Job %d started\n", counter);

    for(i=0; i<(0xFFFFFFFF);i++);

    printf("\n Job %d finished\n", counter);

    my_pthread_mutex_unlock(&lock);

    return NULL;
}

int main(void)
{   
    
    printf("Starting Testing\n");


    printf("Allocating space for the thread array\n");

    thread_list = malloc(NTHREADS * sizeof(my_pthread_t));

    printf("Initializing the Scheduler\n");
    
    printf("Initializing the Mutex\n");
    mutex0 = malloc(sizeof(my_pthread_mutex_t));

    my_pthread_mutex_init(mutex0,NULL);

    printf("Initializing thread\n");

    time_t t;
    long int i;
    long int base = 100;
    long int random[NTHREADS];
    long int random_sec[NTHREADS];

    start = getTime();
    init_schedule();

    srand((unsigned) time(&t));

    for (i = 0; i < NTHREADS; i++) {
        random[i] = rand() % 1000 * base;
        printf("Random Number %li\n", random[i]);
    }   

    thread_list = malloc(NTHREADS * sizeof(my_pthread_t));
    for (i = 0; i < NTHREADS; i++) {
        if (my_pthread_create(&thread_list[i], NULL, doSomeThing, (void *)random[i]) != 0) {
            printf("Error Creating Thread %li\n", i);
        }
    }

        if (my_pthread_create(&thread_list[NTHREADS], NULL, doSomeThing, NULL)) {
            printf("Error Creating Thread %li\n", NTHREADS-4);
    }

    return 0;
}