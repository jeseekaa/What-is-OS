/* BY: Douglas Judice, Shrey Desai, Jessica Guo --- ilab = basic.cs.rutgers.edu */

#include "my_pthread_t.h"
#include <errno.h>
#define ACTIVE 1 //everything in running queue (running queue)
#define YIELD 2 //things that did joins or yields for another thread (waiting queue)
#define RUNNABLE 3 //ran once and now in waiting queue (waiting queue)
#define BLOCKING 4 //thread that is waiting for a mutex (waiting queue)
#define DEAD 5 //threads that have been terminated
#define WAITING 6
static int firstTime = 1;
int nodeID = 0;
my_pthread_t *mainThread;
my_pthread_t *currThread;
priorityQueue *pthreadQueue; // 0 = top level running queue, 1 = bottom level running queue, 2 = waiting queue
//priorityQueue *waitQueue;

/*
 void createQueue(priorityQueue * queue)
 {
 //struct priorityQueue* queue = (struct priorityQueue*)malloc(sizeof(struct priorityQueue));
 queue->front = NULL;
 queue->rear = NULL;
 queue->numNodes = 0;
 return;
 //return queue;
 }
 */

void createThreadNode(my_pthread_t **node)
{
    //ucontext_t context = (ucontext_t *)malloc(sizeof(ucontext_t));
    *node = (my_pthread_t *)malloc(sizeof(my_pthread_t)); 
    (*node)->id = nodeID;
    nodeID++;
    (*node)->waitingOnID = -1;
    (*node)->activeFlag = ACTIVE;
    (*node)->context.uc_stack.ss_sp = (char *)malloc(sizeof(char) * 16384);
    (*node)->context.uc_stack.ss_size = 16384;
    (*node)->next = NULL;
    (*node)->prev = NULL;
    //return node;
}

void enqueue(my_pthread_t * temp, int num){
    
    // num 0=TOP, 1=BOTTOM, 2=WAIT
    //printf("in enqueue: About to enqueue thread with id -> %d, into queue %d\n", temp->id, num); 
    // increment num nodes within Queue
    int i = pthreadQueue[num].numNodes;
    temp->next = NULL;
    i++;
    pthreadQueue[num].numNodes = i;
    
    // if queue is empty
    //printf("in enqueue: before conditionals\n");
    if (pthreadQueue[num].front == NULL)
    {
        pthreadQueue[num].front = temp;
        pthreadQueue[num].rear = pthreadQueue[num].front;
    }
    else
    {
        pthreadQueue[num].rear->next = temp;
        temp->prev = pthreadQueue[num].rear;
        pthreadQueue[num].rear = temp;
        //pthreadQueue[num].rear->next = pthreadQueue[num].front;
        //pthreadQueue[num].front->prev = pthreadQueue[num].rear;
    }
    //printf("in enqueue: returning!\n");
    return;
}

my_pthread_t* dequeue(int num){
    
    // throw error if no threads to dequeue
    if (pthreadQueue[num].front == NULL){
        //printf("in dequeue: No threads in queue! \n");
        return NULL;
    }
    
    // decrement the numNodes in queue
    int i = pthreadQueue[num].numNodes;
    i--;
    if(i <= 0)
        //printf("in dequeue: Last node dequeued! \n");
    pthreadQueue[num].numNodes = i;
    
    // return front
    my_pthread_t *temp = pthreadQueue[num].front;
    pthreadQueue[num].front = pthreadQueue[num].front->next;
    //pthreadQueue[num].rear->next = pthreadQueue[num].front;
    //pthreadQueue[num].front->prev = pthreadQueue[num].rear;
    
    // catch error
    if (pthreadQueue[num].front == NULL)
        pthreadQueue[num].rear == NULL;
    
    return temp;
}

void printQueue(int num){
	printf("viewing pthreadQueue[%d]\n", num);
	my_pthread_t *tmp = pthreadQueue[num].front;
	while(tmp!=NULL){
		printf("id = %d, activeFlag = %d\n", tmp->id, tmp->activeFlag);
		tmp = tmp->next;
	}
}

void initQueue(){
	printf("in initQueue, inside of NULL\n");
	pthreadQueue = malloc(sizeof(priorityQueue)*4);
    	int i = 0;
    	for (i = 0; i < 4; i++) {
        	pthreadQueue[i].front = NULL;
        	pthreadQueue[i].rear = NULL;
        	pthreadQueue[i].numNodes = 0;
    	}
    	//waitQueue = malloc(sizeof(priorityQueue));
    	//createQueue(waitQueue);
    	return;
}

long int timeStamp(){
    struct timeval curr;
    gettimeofday(&curr,NULL);
    curr.tv_sec;
    curr.tv_usec; // microseconds
    return curr.tv_usec;
}

int checkIfEmpty(pthread_retval *head){
	pthread_retval *rt = head;
	while(rt!=NULL){
		//printf("IN CHECKIFEMPTY: id is %d\n", rt->id);
		if(rt->flag == 1){
			return 0;
		}
		rt=rt->next;
	}
	//printf("IN CHECKIFEMPTY: about to return 1\n"); 
	return 1;
}

void valuePlacer(){
	pthread_retval *ret = currThread->returnList;
	pthread_retval *tmp = ret;
	while(tmp!=NULL){
		printf("STUCK INSIDE TMP!!\n");
		my_pthread_t *threads = pthreadQueue[3].front;
		while(threads!=NULL){
			printf("STUCK INSIDE THREADS (%d)!\n", threads->id);
			
			if(threads->id == tmp->id &&  threads->activeFlag == DEAD){
				tmp->waitingForValuePtr = &(threads->valuePtr);
			}
			threads = threads->next;
		}
		tmp = tmp->next;
	}
	currThread->returnList = ret;
}
void checker(){
	pthread_retval *retlist = currThread->returnList;
	while(retlist != NULL){
		void* l = *(retlist->waitingForValuePtr);
		printf("(%d)retlist address = %p and value = %d\n", retlist->id, &retlist->waitingForValuePtr, *(int*)l);
		retlist = retlist->next;
	}
}
void scheduler()
{
    //printf("inside scheduler!!\n");
    
    printQueue(0);
    printQueue(1);
    printQueue(2);
    printQueue(3);
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 50000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    //printf("in scheduler: about to assign head and parent\n");
    /*if(currThread!=NULL){
	//printf("hellow i am thread %d\n", currThread->id);    
	//valuePlacer();
	if(currThread->id == 0){
		//checker();
	}
    }*/
    my_pthread_t* head = pthreadQueue[1].front;
    my_pthread_t* parent = NULL;
    long int curr = timeStamp();
    my_pthread_t* temp = currThread;
    /*check if waiting queue has anything active*/
    my_pthread_t* wait = pthreadQueue[2].front;
    while(head != NULL)
    {
	//printf("in scheduler: LOOPING (head!=NULL): id = %d\n", head->id);
	if(curr - head->lastTimeFinished > (50000*20)){
           // printf("inside more than 50000*20\n");
	    if(parent == NULL){
                pthreadQueue[1].front = head->next;
            }else{
                parent->next = head->next;
            }
            head->activeFlag = ACTIVE;
            enqueue(head, 0);
        } else {
            parent = head;
        }
        head = head->next;
    }
    parent = NULL;    
    if(temp != NULL)
    {
        //temp->totalRuntime += curr - temp->startTime; // Time Quantum = 50K
        //printf("in scheduler(inside of temp!=NULL): flag = %d, id = %d\n", temp->activeFlag, temp->id);
	switch(temp->activeFlag){
	    case 6:
		printf("in scheduler: I AM JOINING AKA WAITING\n");
            	enqueue(currThread, 2);
		break;
	    case 5:
                printf("in scheduler: (%d) I AM DEAD x(\n", currThread->id);
		//free(temp); // clear up this thread
		
		//enqueue(currThread,0);
		int k = 0;
		int l = 0;
		my_pthread_t *check = pthreadQueue[3].front;
		while(check!=NULL){
			if(check->id == temp->id){
				l=1;
				break;
			}
			check=check->next;
		}
		if(l==0){
			enqueue(temp,3);
		}
		while(wait!=NULL)
    		{
		    	
		    printf("in scheduler: LOOPING (wait!=NULL): id = %d\n", wait->id);
        	    //printf("inscheduler: LOOPING (wait!=NULL): wait->waitingID = %d, temp->id = %d\n", wait->waitingOnID, temp->id);
		    pthread_retval *list = wait->returnList;
		     while(list!=NULL){
			printf("IN HERE LIST ->ID = %d\n", list->id);
			   if(list->id == temp->id){
            			printf("SOMETHING CHANGED FROM WAITING TO ACTIVE!\n");
            			if(parent == NULL){
                			pthreadQueue[2].front = wait->next;
            			}else{
                			parent->next = wait->next;
            			}
				printf("%d: currThread->valuePtr = %p\n", currThread->id, &(currThread->valuePtr));
                        	printf("currThread->valuePtr actual value = %d\n", currThread->valuePtr);
				printf("BEFORE: list->waitForValuePtr = %p\n", list->waitingForValuePtr);
				if(list->waitingForValuePtr != NULL && list->waitingForValuePtr != 0){
					*(list->waitingForValuePtr) = &(currThread->valuePtr);
				}
				list->flag = 0;
				printf("AFTER: list->waitForValuePtr = %p\n", list->waitingForValuePtr);

				pthread_retval *rt = wait->returnList;
				while(rt!=NULL){
                                	if(rt->id == currThread->id && rt->waitingForValuePtr != 0){
						printf("%d: wait->valuePtr = %d\n", wait->id, *(rt->waitingForValuePtr));
					}
					rt= rt->next;	
                        	}	
				k=1;
				if(checkIfEmpty(wait->returnList)==1){
					wait->activeFlag = ACTIVE;
					if(pthreadQueue[0].front == NULL){
					    pthreadQueue[0].front = wait;
					    wait->next = NULL;
					} else if(wait->id != pthreadQueue[0].front->id){
					    wait->next = pthreadQueue[0].front;
					    pthreadQueue[0].front = wait;
					}
					printf("about to run (%d)\n", pthreadQueue[0].front->id);
					scheduler();
				

				}
        	    	}
			list = list->next;
		    } 
		    if(k==0){
            		parent = wait;
        	    }
		    k=0;
        	    wait = wait->next;
    		}
		break;
            case 4:
                printf("in scheduler: I AM BLOCKED WHAT TO DO?! id = %d\n", currThread->id);
            	break;
	    case 2:
		printf("in scheduler: in YIELD\n");
                temp->activeFlag = ACTIVE;
                enqueue(temp, 0); // place yielded thread in back of top-level priority queue
            break;
	    default: // place into lower priority queue
                temp->activeFlag = RUNNABLE;
		//printf("in scheduler: in default\n");
		enqueue(temp, 1);
        }
        // Now, pick a new thread to run
        my_pthread_t* next_node = dequeue(0);
        if(next_node!=NULL){
	    printf("in scheduler: next node in first if condition = %d\n", next_node->id); 
	}
	if(next_node == NULL){
            //printf("## No new thread to run in top level 0 queue! \n");
            if(pthreadQueue[1].front != NULL){
                next_node = dequeue(1);
            }
            else{
                //printf("## No new thread to run in bottom level 1 queue! \n");
            }
        }
        currThread = next_node;
    }
    else{
	//printf("in scheduler: about to dequeue\n");
        my_pthread_t* m = dequeue(0);
	m->activeFlag = ACTIVE;
	enqueue(m, 0);
	my_pthread_t *next_node = dequeue(0);
        //printf("m id = %d\n", m->id);
	//printf("next_node id = %d\n", next_node->id);
	//printf("in scheduler: finished dequeue\n");
	if(next_node == NULL){
            //printf("## No new thread to run in top level 0 queue! \n");
            if(pthreadQueue[1].front != NULL){
                next_node = dequeue(1);
            }
            else{
                //printf("## No new thread to run in bottom level 1 queue! \n");
            }
        }
        currThread = next_node;
    }
    
    // SET TIMER STUFF HERE ???
    //printf("in scheduler: STARTING TIMER!!!\n");
    setitimer(ITIMER_REAL, &timer, NULL);
    if(currThread != NULL){
        // TIMESTAMP INFO
        if(temp != NULL){
	    //printf("in scheduler: thing NOT last else codition\n");
            temp->lastTimeFinished = curr;
	    printf("currthread id = %d, temp id = %d\n", currThread->id, temp->id);
            swapcontext(&(temp->context), &(currThread->context));
		printf("^ Swap successful ^ \n");
        }
        else{
	    printf("in scheduler: last else condition\n");
            swapcontext(&(mainThread->context), &(currThread->context));
	    //printf("made it here??\n");
        }
    }
    return;
}

void exitChecker(my_pthread_t* thread, void *(*function)(void *), void *arg){
	currThread = thread;
	printf("$$$$$$$$ %d: address of thread->valuePtr BEFORE v = %p\n", thread->id, &(thread->valuePtr));
	//void* v = function(arg);
	//printf("$$$$$$$BUT what is function(arg)?%d\n", function(arg));
	//printf("$$$$$$$ v = %d\n", v);
	thread->valuePtr = function(arg);
	//printf("$$$$$$$$$$$$$$ %d: AFTER VALUE POINT = %p\n", thread->id, &(thread->valuePtr));
	//printf("~~~~~IN EXITCHECKER VALUEPTR = %d\n", thread->valuePtr);
	if(thread->activeFlag != DEAD){
		thread->activeFlag = DEAD;
	}
	scheduler();
}

int my_pthread_create( my_pthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg)
{
    signal(SIGALRM, scheduler);    
    if(firstTime == 1)
    {
        //printf("in create: inside firstTime\n"); 
	initQueue();
        //pthreadQueue = createQueue();
        createThreadNode(&mainThread);
	mainThread->activeFlag = ACTIVE;
	//printf("in create: about to enqueue\n");
	enqueue(mainThread, 0);
        firstTime = 0; // designate that return to create are not the first time
    }
    //printf("in create: outside of firstTime\n");
    //createThreadNode(&thread);
    thread->id = nodeID;
    nodeID++;
    thread->waitingOnID = -1;
    thread->activeFlag = ACTIVE;
    thread->context.uc_stack.ss_sp = (char *)malloc(sizeof(char) * 16384);
    thread->context.uc_stack.ss_size = 16384;
    thread->next = NULL;
    thread->prev = NULL;
    printf("in create: outside of createThreadNode()\n");
    thread->activeFlag = ACTIVE;    
    printf("******IN CREATE: THREAD->ID = %d ADDRESS OF thread.ID = %p\n", thread->id, (void*)&thread->id);
    getcontext(&(thread->context));
    makecontext(&(thread->context), (void *)exitChecker, 3, thread, function, arg);
    //printf("in create: before enqueue\n");
    enqueue(thread, 0);
    scheduler();
    return 0;
}

// Explicit call to the my_pthread_t scheduler requesting that the current context be swapped out and another scheduled
void my_pthread_yield(){
	printf("*** IN PTHREAD YIELD *** \n\n");    
    //my_pthread_t node;
    currThread->activeFlag = YIELD;
    scheduler();
    //my_pthread_t nextThread = start_next_thread(); // start_next_thread goes through the pthreadQueue, gets next in line, changes WAITING/ACTIVE flags
    
    //swapcontext(currThread->context, nextThread->context);
    
    return;
}

// Explicit call to the my_pthread_t library to end the pthread that called it. if value_ptr isn't NULL any return val from thread will be saved
void my_pthread_exit(void *value_ptr){
    //make sure to account for value_ptr (return value)
    printf("********IN EXIT ORIGINAL VALUE = %d\n", value_ptr);
    if(currThread->activeFlag != DEAD){
        currThread->activeFlag = DEAD;
	currThread->valuePtr = value_ptr;
   	printf("*************IN EXIT VALUE POINT = %d\n", currThread->valuePtr); 
   }
    //free(currThread->context);
    
    scheduler();
}

int my_pthread_join(my_pthread_t thread, void **value_ptr){
    //thread is the thread that current thread wants to wait till exit
    int targetID = thread.id;
    //my_pthread_yield();
    pthread_retval *rt = malloc(sizeof(pthread_retval));
    rt->flag = 1;
    //currThread->activeFlag = WAITING;
    //currThread->waitingOnID = targetID;
    printf("$$$$$$ [%d] **valu_ptr = %p\n", thread.id, value_ptr);
    /*if(value_ptr == NULL){
	if(thread.activeFlag != DEAD){
		currThread->activeFlag = WAITING;
		//currThread->waitingOnID = targetID;
		rt->flag = 1; 
		rt->id = targetID;
		rt->next = NULL;
		
		if(currThread->returnList == NULL){
        		printf("IN JOIN IN NULL PART for thread.id = %d\n", thread.id);
        		rt->id = targetID;
        		rt->waitingForValuePtr = value_ptr;
        		rt->next = NULL;
        		currThread->returnList = rt;
    		} else {
        		rt->id = targetID;
        		rt->waitingForValuePtr = value_ptr;
        		pthread_retval *head = currThread->returnList;
        		pthread_retval *tmp = head;
        pthread_retval *n = head->next;
        int j = 0;
        while(n!=NULL){
                j++;
                tmp=n;
                n=n->next;
        }
        tmp->next = rt;
        currThread->returnList = head;
    }

	}
	scheduler();
	return NULL;
    }*/
    if(thread.activeFlag == DEAD){
    	printf("in here\n");
	if(value_ptr !=NULL){
		*(value_ptr) = &(thread.valuePtr);
	}
	//printf("IN JOIN: %d\n", **(int**)(value_ptr));
    } else {
    	currThread->activeFlag = WAITING;
        currThread->waitingOnID = targetID;
        rt->flag = 0;
    
    if(currThread->returnList == NULL){
	printf("IN JOIN IN NULL PART for thread.id = %d\n", thread.id);
    	rt->id = targetID;
    	rt->waitingForValuePtr = value_ptr;
    	rt->next = NULL;
    	currThread->returnList = rt;
    } else {
        rt->id = targetID;
        rt->waitingForValuePtr = value_ptr;
	pthread_retval *head = currThread->returnList;
        pthread_retval *tmp = head;
	pthread_retval *n = head->next;
	int j = 0;
	while(n!=NULL){
		j++;
		tmp=n;
		n=n->next;
	}
	tmp->next = rt;	
	currThread->returnList = head;
    }
    }
    //checker();
    /*pthread_retval *ret = currThread->returnList;
    while(ret!=NULL){
    	printf("$$$$$$ [%d] currThread->value ptr = %p\n", ret->id, ret->waitingForValuePtr);
    	ret = ret->next;
    }*/
    scheduler();
    //make sure to account for value_ptr (return value)
    return NULL;
    
}


/*
 
 QUESTIONS:
 
 */

/*
 IMPORTANT STUFF:
 - signal(SIGALRM, some schedule handler method)
 - Keep copy of main in global variable??
 - For join add a flag that shows wat the current thread is waiting for?
 - two queues (running and waiting)
 - running: only things in here get to run one after the other for 50 ms.
 once something runs for 50ms and doesn't finished it gets moved to waiting queue.
 - Any new thread gets immediately put in here.
 - waiting: NOTHING HERE GETS RUN but there can be things in here that are "RUNNABLE".
 there are threads that ran but DID NOT finish so we lowered its priority by putting it in the waiting queue.
 What this does is if we fully run through the running queue then only will we go to the waiting queue
 and look for things that are "RUNNABLE"
 */

void gen_create_queue(priorityQueue * q){
    
    q->front = NULL;
    q->rear = NULL;
    q->numNodes =0;
}

void gen_enqueue(priorityQueue * q, my_pthread_t * thread_node){
    
    if(q->numNodes ==0){ //empty queue
        q->front = thread_node;
        q->rear = thread_node;
        q->numNodes++;
    }
    else{ // FIFO, add to tail
        q->rear->next = thread_node;
        thread_node->prev = q->rear;
	q->rear = thread_node;
        q->numNodes++;
        
    }
}

my_pthread_t * gen_dequeue(priorityQueue * q){
    
    //check empty
    if(q->numNodes ==0){
        printf("Empty Queue\n");
        return NULL;
    }
    
    //FIFO, dequeue from head, create a temp to return the value
    my_pthread_t * temp;
    if(q->numNodes ==1){
        temp = q->front;
        q->front = NULL;
        q->rear = NULL;
    }
    else{
        temp = q->front;
        q->front = q->front->next;
    }
    
    q->numNodes--;
    
    return temp;
    
}


int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr){
    /*Initializes a my_pthread_mutex_t created by the calling thread. Attributes are ignored.*/
    int init = 1;
    printf("start initializing mutex\n");
    //mutex = malloc(sizeof(my_pthread_mutex_t));
    
    if(mutex == NULL){
        return EINVAL;
    }
    
    mutex->flag = 0;
    mutex->block = 0;
    mutex->wait = malloc(sizeof(priorityQueue)); //make a waiting queue for mutex
    
    mutex->wait->front = NULL;
    mutex->wait->rear = NULL;
    mutex->wait->numNodes = 0;
    
    printf("mutex was succuesfully initiated\n");
    
    return init=0;
}

int my_pthread_mutex_lock(my_pthread_mutex_t *mutex){
    /*Locks a given mutex, other threads attempting to access this mutex will not run until it is unlocked.*/
    //printQueue(0);
    //printQueue(1);
    //printQueue(2);
    int locking = 1;
    printf("pthread is being locked by %d\n", currThread->id);
    //printf("mutex->lock = %d\n", mutex->flag); 
    while(__sync_lock_test_and_set(&(mutex->flag),1)==1){
        printf("inside synlocktestandset loop\n");
        currThread->activeFlag = BLOCKING; // set the state of the current thread to be WAITITNG
        gen_enqueue(mutex->wait, currThread); //this thread should be waiting in the mutex queue
    	//printf("viewing mutex->wait queue\n");
        my_pthread_t *tmp = mutex->wait->front;
        while(tmp!=NULL){
                printf("id = %d, activeFlag = %d\n", tmp->id, tmp->activeFlag);
                tmp= tmp->next;
        }
	//enqueue(currThread, 2);
	scheduler();
    }
    
    //printf(" number in mutex queue: %d\n", mutex->wait->numNodes);
    
    if (mutex->flag ==0){
        mutex->flag =1;
    }
    
    //printf("mutex flag: %d\n", mutex->flag);
    printf("pthread locked\n");
    
    return locking=0;
}

int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex){
    /*Unlocks a given mutex.*/
    int unlock =1;
    //printf("pthread is being unlocked by thread with id =%d\n", currThread->id);
    
    my_pthread_t * target_thread;
    
    //printf("%d\n", mutex->wait->numNodes);
    
    if(mutex->wait->front != NULL){
        printf("inside unlcok's if state,ent\n");
        target_thread = gen_dequeue(mutex->wait);
	
	target_thread->activeFlag = ACTIVE;
	enqueue(target_thread, 0);
        printf("mutex is available\n");
        
    }
    
    mutex->flag = 0;
    
    printf("pthread unlocked\n");
    scheduler(); 
    return unlock = 0;
    
}


int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex){
    /*Destroys a given mutex. Mutex should be unlocked before doing so.*/
    int destroy =1;
    printf("pthread is being destoryed\n");
    
    if(mutex ==NULL){
        return EINVAL;
    }
    
    if(mutex->flag != 0){
        return EBUSY;
    }
    //unlock the mutex
    mutex->flag =0;
    
    //free(mutex);
    
    printf("mutex destroyed\n");
    
    return destroy=0;
}

