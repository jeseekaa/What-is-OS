#include <stdlib.h>
#include <sys/mman.h>

#define MEM_SIZE 8388608
#define SWP_SIZE 16777612
#define PAGE_SIZE 4096 //derived from sysconf(_SC_PAGE_SIZE)??
#define LIBRARYREQ 0
#define THREADREQ 1
#define SYSTEMREQ 2
#define SWP_PAGE_CNT 4096

#define malloc(x) myallocate(x, __FILE__, __LINE__, THREADREQ)
#define free(x) mydeallocate(x, __FILE__, __LINE__, THREADREQ)

typedef struct _mem_data{
    int id;
    size_t size;//heap_size
    int memFree;
    int remainingSpace;
    int startPageIndex;
    int lastPageIndex;
    //page_info* stack;//stack
    //page_info* head;//heap
    //page_info* tail;
    struct _mem_data* prev;
    struct _mem_data* next;
    char dataPtr[1];
} mem_data;



void * myallocate(size_t size, char * fileName, int lineNumber, int requestID);
void mydeallocate(void * block, char * fileName, int lineNumber, int requestID);