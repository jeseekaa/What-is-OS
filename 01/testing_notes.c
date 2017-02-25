/*
 *
 * These are the function and the main I added to test threads before I get to testing mutexes.
 *
 * It is threading.c but using the my_pthread_t functions.
 *
 */


 void *myfunc(void *myvar){

    printf("in myfunc\n");

    char *msg;
    msg =(char*)myvar;

    printf("checkpoint in myfun\n");

    int i;
    for(i=0; i<10; i++){
        printf("checkpoint: myfunc's for loops\n");
        printf("loop %d\n", i);
        printf("%s %d \n", msg, i);
        sleep(1);
    }

    return NULL;
 }


int main(){

    my_pthread_t thread1, thread2;

    printf("pthread structs created\n");

    char * msg1 = "First thread";
    char * msg2 = "Second thread";

    int ret1, ret2;

    ret1 = my_pthread_create(&thread1, NULL, myfunc, (void*)msg1);
    ret2 = my_pthread_create(&thread2, NULL, myfunc, (void*)msg2);

    printf("back in main after pthread creation\n");


    my_pthread_join(thread1, NULL);
    my_pthread_join(thread2, NULL);

    printf("pthreads have joined\n");

    printf("first thread ret = %d\n", ret1);
    printf("second thread ret = %d\n", ret2);

    return 0;
}

/*
 *  here is are the results after compiling: gcc -o mypthread my_pthread.c my_pthread_t.h
 * 
 *

-bash-4.2$ ./mypthread
pthread structs created
in create: inside firstTime
in create: about to enqueue
in create: outside of firstTime
in create: outside of createThreadNode()
in create: before enqueue
viewing pthreadQueue[0]
id = 0, activeFlag = 1
id = 1, activeFlag = 1
viewing pthreadQueue[1]
viewing pthreadQueue[2]
in scheduler: about to dequeue
m id = 0
next_node id = 1
in scheduler: finished dequeue
in scheduler: STARTING TIMER!!!
in scheduler: last else condition
in myfunc
checkpoint in myfun
checkpoint: myfunc's for loops
loop 0
 0
viewing pthreadQueue[0]
id = 0, activeFlag = 1
viewing pthreadQueue[1]
viewing pthreadQueue[2]
in scheduler(inside of temp!=NULL): flag = 1, id = 1
in dequeue: Last node dequeued!
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 0, temp id = 1
made it here??
in create: outside of firstTime
in create: outside of createThreadNode()
in create: before enqueue
viewing pthreadQueue[0]
id = 2, activeFlag = 1
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 1, id = 0
in dequeue: Last node dequeued!
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 0
in myfunc
checkpoint in myfun
checkpoint: myfunc's for loops
loop 0
 0
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
id = 0, activeFlag = 3
viewing pthreadQueue[2]
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler: LOOPING (head!=NULL): id = 0
in scheduler(inside of temp!=NULL): flag = 1, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 1
 1
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 0, activeFlag = 3
id = 2, activeFlag = 3
viewing pthreadQueue[2]
in scheduler: LOOPING (head!=NULL): id = 0
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 0, temp id = 1
back in main after pthread creation
in join: thread->id = 0
, flag = 0
in join: targetID = 0
in join: joinedThread->id = 1946271875
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
id = 1, activeFlag = 3
viewing pthreadQueue[2]
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 6, id = 0
in scheduler: I AM JOINING AKA WAITING
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 0
checkpoint: myfunc's for loops
loop 1
 1
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 2
 2
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 2
 2
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 3
 3
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 3
 3
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 4
 4
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 4
 4
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 5
 5
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 5
 5
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 6
 6
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 6
 6
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 7
 7
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 7
 7
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 8
 8
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 8
 8
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2
checkpoint: myfunc's for loops
loop 9
 9
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 2, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 2
in scheduler(inside of temp!=NULL): flag = 3, id = 1
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 2, temp id = 1
checkpoint: myfunc's for loops
loop 9
 9
viewing pthreadQueue[0]
viewing pthreadQueue[1]
id = 1, activeFlag = 3
viewing pthreadQueue[2]
id = 0, activeFlag = 6
in scheduler: LOOPING (head!=NULL): id = 1
in scheduler(inside of temp!=NULL): flag = 3, id = 2
in scheduler: STARTING TIMER!!!
in scheduler: thing NOT last else codition
currthread id = 1, temp id = 2

*
*
*
* YAY threads and scheduler are working! Mad props!
*
* Problems :(
*
* IN MAIN:
    printf("pthreads have joined\n");

    printf("first thread ret = %d\n", ret1);
    printf("second thread ret = %d\n", ret2);
 * 
 * are not being printed for some reason
 *
 * IN MYFUNC:
 *
 * printf("%s %d \n", msg, i);
 * 
 * is also being printed weirdly (?? I think this is on my part but I'm not sure)
 *
 *
 */