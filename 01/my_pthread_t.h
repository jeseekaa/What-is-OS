/*
Shrey Desai
Jessica Guo
Douglas Justice

Assignment 01 due 02/26/2017 11:55PM
*/

#idndef MY_PTHREAD_T_H
#define MY_PTHREAD_T_H

#define NTHREADS 10 
#define STACK_SIZE 32768 /* 32kb stack */
#define NLEVELS 20
#define NLOCKS 1
#define TIME_QUANT 50000
#define AGE_THRSHLD 2000000
#define CHECK_FRQ 50

typedef enum my_pthread_state {
	NEW, READY, RUNNING, WAITING, TERMINATED, YIELD
} my_pthread_state;

/*pthread struct*/
typedef struct my_pthread_t {

	ucontext_t context_ptr; //pointer to the context that will resume when this conext returns

	struct my_pthread_t * nxt_pthread;
	my_pthread_state thread_state;
	long int pthread_id;
	int num_runs;
	int time_ran;
	int priority;
	void * return_value;
	long int start_tt;
	long int first_exe_tt;
	long int last_exe_tt;
	long int end_tt;

} my_pthread_t;


/*mutex struct*/
typedef struct my_pthread_mutex_t{
	volatile int flag; 
	volatile int block; //volatile?
	my_pthread_t owner_thread;
	queue * wait;

} my_pthread_mutex_t;

/*queue struct and basic functions: create, enqueue, dequeue, peek, isEmpty*/
typedef struct{
	my_pthread_t * head;
	my_pthread_t * tail;
	int size;
} queue;

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

/* Scheduler struct and Scheduling functions*/
typedef struct schedule{
	queue * MLPqueue;
	queue * wait;
	my_pthread_t * thread_main;
	my_pthread_t * thread_curr;
	int priority_list[NLEVELS];
	long int num_sched;
} scheduler;

void run_mypthread();
void adjust_age();
void init_schedule();
void add_mypthread_schedule();
my_pthread_t * sched_chooseThread();
void schedule_handler();

/*mutex functions*/

int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
/*Initializes a my_pthread_mutex_t created by the calling thread. Attributes are ignored.*/
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);
/*Locks a given mutex, other threads attempting to access this mutex will not run until it is unlocked.*/
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);
/*Unlocks a given mutex.*/
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);
/*Destroys a given mutex. Mutex should be unlocked before doing so.*/



/*static variables*/
static my_pthread_mutext_t * mutex0;
static my_pthread_t * thread_list;
static scheduler * sched;

static long int thread_id =0;
static long int check_flag =0;

static int sharedVar0 =0;
static int shared Var1 =0;

static int start =0;
static int end =0;


#endif
