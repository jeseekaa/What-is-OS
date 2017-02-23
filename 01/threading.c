//pthread 

//compile with gcc -o threading -pthread threading.c



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

 void *myfunc(void *myvar){
 	char *msg;
 	msg =(char*)myvar;

 	int i;
 	for(i=0; i<10; i++){
 		printf("%s %d\n", msg, i);
 		sleep(1);
 	}

 	return NULL;
 }

int main(int argc, char*argv[]){
 
 //pthread_create has four arguments
	pthread_t thread1, thread2;

	char* mesg1 = "First thread";
	char* mesg2 = "Second thread";

	int ret1, ret2;

	ret1 = pthread_create(&thread1, NULL, myfunc, (void*)mesg1);
	ret2 = pthread_create(&thread2, NULL, myfunc, (void*)mesg2);

	printf("Main funtion after pthread created\n");

	//we have tp join the threads after

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("first thread ret = %d\n", ret1);
	printf("second thread ret = %d\n", ret2);


	return 0;	
}
 
