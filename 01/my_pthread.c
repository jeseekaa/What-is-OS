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

my_pthread_t* createThreadNode(my_pthread_t *node)
{
    //ucontext_t context = (ucontext_t *)malloc(sizeof(ucontext_t));
    node = (my_pthread_t*)malloc(sizeof(my_pthread_t)); 
    node->id = nodeID;
    nodeID++;
    node->waitingOnID = -1;
    node->activeFlag = ACTIVE;
    node->context.uc_stack.ss_sp = (char *)malloc(sizeof(char) * 16384);
    node->context.uc_stack.ss_size = 16384;
    node->next = NULL;
    node->prev = NULL;
    return node;
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
        printf("in dequeue: Last node dequeued! \n");
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
    pthreadQueue = malloc(sizeof(priorityQueue)*3);
    int i = 0;
    for (i = 0; i < 3; i++) {
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

void scheduler()
{
    //printf("inside scheduler!!\n");
    
    printQueue(0);
    printQueue(1);
    printQueue(2);
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 50000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    //printf("in scheduler: about to assign head and parent\n");
    my_pthread_t* head = pthreadQueue[1].front;
    my_pthread_t* parent = NULL;
    long int curr = timeStamp();
    my_pthread_t* temp = currThread;
    /*check if waiting queue has anything active*/
    my_pthread_t* wait = pthreadQueue[2].front;
    while(head != NULL)
    {
	printf("in scheduler: LOOPING (head!=NULL): id = %d\n", head->id);
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
        printf("in scheduler(inside of temp!=NULL): flag = %d, id = %d\n", temp->activeFlag, temp->id);
	switch(temp->activeFlag){
	    case 6:
		printf("in scheduler: I AM JOINING AKA WAITING\n");
		enqueue(temp,2);
            	break;
	    case 5:
                printf("in scheduler: I AM DEAD x(\n");
		//free(temp); // clear up this thread
		//enqueue(currThread,0);
		while(wait!=NULL)
    		{
        	    printf("in scheduler: LOOPING (wait!=NULL): id = %d\n", wait->id);
        	    printf("inscheduler: LOOPING (wait!=NULL): wait->waitingID = %d, temp->id = %d\n", wait->waitingOnID, temp->id);
		    if(wait->waitingOnID == temp->id){
            		printf("SOMETHING CHANGED FROM WAITING TO ACTIVE!\n");
            		if(parent == NULL){
                		pthreadQueue[2].front = wait->next;
            		}else{
                		parent->next = wait->next;
            		}
			wait->activeFlag = ACTIVE;
            	        enqueue(wait, 0);
        	    } else {
            		parent = wait;
        	    }
        	    wait = wait->next;
    		}
		break;
            case 4:
                //printf("## Thread should already be in the Wait Queue\n ##"); // should be done in mutex lock function
            	break;
	    case 2:
		//printf("in scheduler: in YIELD\n");
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
        //printf("in scheduler: next node in first if condition = %d\n", next_node->id); 
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
	printf("in scheduler: about to dequeue\n");
        my_pthread_t* m = dequeue(0);
	m->activeFlag = ACTIVE;
	enqueue(m, 0);
	my_pthread_t *next_node = dequeue(0);
        printf("m id = %d\n", m->id);
	printf("next_node id = %d\n", next_node->id);
	printf("in scheduler: finished dequeue\n");
	if(next_node == NULL){
            //printf("## No new thread to run in top level 0 queue! \n");
            if(pthreadQueue[1].front != NULL){
                next_node = dequeue(1);
            }
            else{
                printf("## No new thread to run in bottom level 1 queue! \n");
            }
        }
        currThread = next_node;
    }
    
    // SET TIMER STUFF HERE ???
    printf("in scheduler: STARTING TIMER!!!\n");
    setitimer(ITIMER_REAL, &timer, NULL);
    if(currThread != NULL){
        // TIMESTAMP INFO
        if(temp != NULL){
	    printf("in scheduler: thing NOT last else codition\n");
            temp->lastTimeFinished = curr;
	    printf("currthread id = %d, temp id = %d\n", currThread->id, temp->id);
            swapcontext(&(temp->context), &(currThread->context));
        }
        else{
	    printf("in scheduler: last else condition\n");
            swapcontext(&(mainThread->context), &(currThread->context));
	    printf("made it here??\n");
        }
    }
    return;
}


int my_pthread_create( my_pthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg)
{
    signal(SIGALRM, scheduler);    
    if(firstTime == 1)
    {
        printf("in create: inside firstTime\n"); 
	initQueue();
        //pthreadQueue = createQueue();
        mainThread = createThreadNode(mainThread);
        mainThread->activeFlag = ACTIVE;
	printf("in create: about to enqueue\n");
	enqueue(mainThread, 0);
        firstTime = 0; // designate that return to create are not the first time
    }
    printf("in create: outside of firstTime\n");
    thread = createThreadNode(thread);
    printf("in create: outside of createThreadNode()\n");
    thread->activeFlag = ACTIVE;    
    getcontext(&(thread->context));
    makecontext(&(thread->context), (void (*)()) function, 2, thread, arg);
    printf("in create: before enqueue\n");
    enqueue(thread, 0);
    scheduler();
    return 0;
}

// Explicit call to the my_pthread_t scheduler requesting that the current context be swapped out and another scheduled
void my_pthread_yield(){
    
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
    if(currThread->activeFlag != DEAD){
        currThread->activeFlag = DEAD;
    }
    //free(currThread->context);
    scheduler();
}

int my_pthread_join(my_pthread_t thread, void **value_ptr){
    //thread is the thread that current thread wants to wait till exit
    int targetID = thread.id;
    int i = 0;
    my_pthread_t* temp;
    my_pthread_t* joinedThread;
    //mypthread_t *current_thread = search_thread(mythread_q, cur_thread_id);
    printf("in join: thread->id = %d\n, flag = %d\n", thread.id, thread.activeFlag);
    printf("in join: targetID = %d\n", targetID);
    temp = pthreadQueue->front;
        while(temp != NULL && i < pthreadQueue->numNodes)
        {
            if (temp->id == targetID)
            {
                joinedThread = temp;
                break;
            }
        
            else
            {
                temp = temp->next;
            }   
            i++;
        }
    printf("in join: joinedThread->id = %d\n", joinedThread->id); 
	    //my_pthread_yield();
	    currThread->activeFlag = WAITING;
	    //currThread->waitingOnID = thread.id;
	    scheduler();
	
	
    
    //make sure to account for value_ptr (return value)
    //schedule
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
