/*
Shrey Desai
Jessica Guo
Douglas Judice

Assignment 01 due 02/26/2017 @ 11:55PM
*/

#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include "my_pthread_t.h"



/*Generic queue functions*/
void create_queue(queue * first);
void enqueue(queue * first, my_pthread_t * thread_node);
my_pthread_t * dequeue(queue * first);
my_pthread_t * peek(queue * first);
char isEmpty(queue *first);


/*pthread library*/ 

int my_pthread_create( my_pthread_t * thread, const pthread_attr_t * attr, void *(*function)(void*), void * arg);
/*Creates a pthread that executes function. Attributes are ignored.*/
void my_pthread_yield();
/*Explicit call to the my_pthread_t scheduler requesting that the current context be swapped out and another be scheduled.*/
void my_pthread_exit(void *value_ptr);
/*Explicit call to the my_pthread_t library to end the pthread that called it. If the value_ptr isn't NULL, any return value from the thread will be saved.*/

int my_pthread_join(my_thread_t thread, void **value_ptr);
/*Call to the my_pthread_t library ensuring that the calling thread will not execute until the one it references exits. If value_ptr is not null, the return value of the exiting thread will be passed back.*/

/* Scheduling functions*/
void init_schedule();
void add_mypthread_schedule();
my_pthread_t * sched_chooseThread();
void run_mypthread();
void adjust_age();

/*mutex functions: Jessica */

int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr){
/*Initializes a my_pthread_mutex_t created by the calling thread. Attributes are ignored.*/
	int init = 0;

	if(mutex == NULL){
		return EINVAL;
	}

	mutex->flag =0;
	mutex->block =0;
	mutex->wait = malloc(sizeof(queue));
	create_queue(mutex->wait);

	return init;
}

int my_pthread_mutex_lock(my_pthread_mutex_t *mutex){
/*Locks a given mutex, other threads attempting to access this mutex will not run until it is unlocked.*/
	int locking = 0;
	//spinlocking
	while(__sync_lock_test_and_set(&(mutex->flag), 1)==1){
		sched ->thread_curre>thread_state = WAITING; //set state of thread to be WAITING on mutex
		enqueue(mutex->wait, sched->thread_curr);

		schedule_handler();
	}

	return locking;
}

int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex){
/*Unlocks a given mutex.*/
	int unlock =0;

	my_pthread_t * target_thread;
	if(mutex->wait->head != NULL){
		target_thread = dequeue(mutex->wait);
		//mutex is available 

		sched_addThread(target_thread, target_thread->priority);
	}

	mutex->flag =0;

	return unlock;

}


int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex){
/*Destroys a given mutex. Mutex should be unlocked before doing so.*/
	int destory =0;

	if(mutex ==NULL){
		return EINVAL;
	}

	if(mutex->flag != 0){
		return EBUSY;
	}

	return destroy;
}





//probs only for testing
int main(){
  return 0;
}
