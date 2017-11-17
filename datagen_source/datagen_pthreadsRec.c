/**
* FILE: pthreadsRec.c
* THMMY, 7th semester, Parallel and Distributed Systems: 1st assignment
* Parallel implementation of bitonic sort
* Authors:
*   Kamtziridis Georgios, 8542, gkamtzir@auth.gr
*   Katomeris Nikolaos  , 8551, ngkatomer@auth.gr
* Compile command with gcc example:
*   gcc pthreadRec.c -O3 -pthread -o <executable file name>
* Usage: ./<executable file name> n q
*   where 2^n is the number of the array elements and 2^q the number of threads.
*   if q >= n, the program will run as if q = n - 1.
* Run command example:
*   ./a.out 24 3
* will sort an array with 2^24 elements using 2^3 threads at most simultaneously
* This program works with arrays up to 2^30 on machines where the integer size
* in memory is 4 bytes.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

//direction macro definitions
#define ASCENDING 1
#define DESCENDING 0

//GLOBAL VARIABLES FOR EASY ACCESS FROM ALL FUNCTIONS
int* array;
int N;
int nt;
int L;
int CALLED_ARRAY_SIZE;
int MAX_ARRAY_SIZE;

//struct to be used as argument at functions called by other threads
struct data{
    int start;
    int cnt;
    int dir;
};
typedef struct data threadData; //For simplicity

//FUNCTION DECLARATIONS
void* split(void * arg);
void* merge(void * threadData);
void* elementCompare(void * threadData);
void* ptQuickSort(void *threadData);
void customQuickSort(int start_index, int cnt, int direction);
int cmpfuncAsc (const void * a, const void * b);
int cmpfuncDes (const void * a, const void * b);
double timeRun(void (*function()), void* argument,
                int printResultFlag, char* const fname);

//Original file bitonic.c functions
void init();
void print();
void test();
inline void exchange(int i, int j);
inline void compare(int i, int j, int dir);
void bitonicMerge(int start, int cnt, int dir);

//main
int main(int argc, char **argv)
{
    //Check main arguments and pass them to the global variables
    if (argc != 4) {
        printf("Usage: %s n q d\nWhere 2^n is the number of the array elements"
			  " and 2^q the number of threads\n. d is the MAX_ARRAY_SIZE\n"
			  "This program should be called from a script\n", argv[0]);
        exit(1);
    }
    int n = atoi(argv[1]);
    int q = atoi(argv[2]);
    //L must be at least two in order for this program to work
    if (q >= n) q = n - 1;
    N = 1 << n;
    nt = 1 << q;
    L = 1 << n - q;

	CALLED_ARRAY_SIZE = n; 
	MAX_ARRAY_SIZE = atoi(argv[3]);

    //Allocate memory for the array
    array = (int *)malloc(N*sizeof(int));
    if (array == NULL) {
        const int MB_size = 8*1024*1024; //1 MB = 1024 KB = 1024 B = 8 bit
        printf("Couldn't allocate memory for the array, probably because of"
                " its size: %fMB\n", (float) N*sizeof(int)/MB_size);
        exit(-1);
    }

    //Initialize the array
    init();

    //Define and initialize a threadData variable for the sorting.
    threadData data = {0, N, ASCENDING};

    //The array has two or less elements or the number of threads is one
    // or less (through incorrect main argument) use qsort for sorting.
    if (nt <= 1 || N <= 2)
    {
        printf("Sorting using one thread and qsort...\n");
        timeRun(ptQuickSort, (void*) &data, 1, "QuickSort");

        return 0;
    }

    //Sort the array with parallel bitonic sort, and time the sort.
    //Check for correctness of the sort.
    timeRun(split, (void*)&data, 1, "Parallel bitonic sort");
	//test();

    //Re-randomize the array and sort it with quicksort, print the sorting time.
    //init();
    //timeRun(ptQuickSort, (void*) &data, 1, "QuickSort");

    //print(); //Uncomment for printing the array elements.

    //Free the allocated memory from the heap.
    free(array);

    //Return 0 indicating successful termination of the program.
    return 0;
}



void* split(void* arg)
{
    threadData data = *((threadData *) arg);

    int k = data.cnt / 2;

	//If data.cnt == L, then we will have one available thread
    //and the sequence from data.start to data.start + data.cnt
    //is random so we sort the elements using customQuickSort().
    if (data.cnt == L) {
        customQuickSort(data.start, data.cnt, data.dir);
    }
    else {
        pthread_t one;
        threadData dataOne = {data.start    , k, ASCENDING};
        threadData dataTwo = {data.start + k, k, DESCENDING};

        pthread_create(&one, NULL, split, (void *)&dataOne);
		split((void*)&dataTwo);

        pthread_join(one, NULL);
        merge((void*)&data);
     }

}


void* merge(void* arg)
{
    threadData data = *((threadData*)arg);

    int k = data.cnt / 2;
    
    //If data.cnt == L, then we will have one available thread
    //and the sequence from data.start to data.start + data.cnt
    //is bitonic so we sort the elements using bitonicMerge().
    if (data.cnt == L){
				bitonicMerge(data.start, data.cnt, data.dir);
    }
    else {
		pthread_t one;
        threadData dataOne = {data.start      , k/2, data.dir};
        threadData dataTwo = {data.start + k/2, k/2, data.dir};

        //Do the half comparisons (possible swaps) on an other thread.
        pthread_create(&one, NULL, elementCompare, (void *)&dataOne);
        //And the other half on this thread.
        elementCompare((void*) &dataTwo);
        //Make sure that the other thread finished the swaps
        pthread_join(one, NULL);

        dataOne.cnt = k;
        dataTwo.start = data.start + k;
        dataTwo.cnt = k;
		
        //Call merge again in the first half elements on another thread
        pthread_create(&one, NULL, merge, (void *)&dataOne);
        //In the same thread for the other half.
        merge((void*)&dataTwo);
        //Make sure the other thread has finished too, before exiting.
        pthread_join(one, NULL);
    }
}

void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}

void* elementCompare(void * args)
{
    threadData data = *((threadData*)args);
    int i;
    for (i = 0; i < data.cnt; i++){
        compare(data.start + i, data.start + data.cnt*2 + i, data.dir);
    }
}

/*-------------------------------Helper methods-------------------------------*/
/**
* Custom qsort() for sorting an array based on a threadData argument
*/
void* ptQuickSort(void *arg)
{
    threadData data = *((threadData*)arg);
    customQuickSort(data.start, data.cnt, data.dir);
}

void customQuickSort(int start_index, int cnt, int direction){
    if (direction == ASCENDING)
        qsort(&array[start_index], cnt, sizeof(int), cmpfuncAsc);
    else
        qsort(&array[start_index], cnt, sizeof(int), cmpfuncDes);
}

/**
* Runs a function and returns its execution time.
* The 3rd and 4th arguments are for choosing to print the result as well
* (<printResultFlag> != 0) with the name of the function <fname>
*/
double timeRun(void (*function()), void* argument,
                int printResultFlag, char* const fname)
{
    struct timeval startwtime, endwtime;

    gettimeofday (&startwtime, NULL);
    function(argument);
    gettimeofday (&endwtime, NULL);
    double exec_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
            + endwtime.tv_sec - startwtime.tv_sec);
            
    if (printResultFlag && CALLED_ARRAY_SIZE != MAX_ARRAY_SIZE)
        printf("%lf, ", exec_time);
    else 
		printf("%lf", exec_time);
		
    return exec_time;
}

/**
* Random array initializing function, works only with the global variables
* <array> and <N> for the array and its size.
*/
void init(){
    int i;
    for (i = 0; i < N; i++){
        array[i] = rand() % N + 1;
    }
}

/**
* Tests if the int array <array> of size <N> is sorted and prints the result.
* Works only for ASCENDING direction.
*/
void test() {
    int pass = 1;
    int i;
    for (i = 1; i < N; i++) {
        pass &= (array[i-1] <= array[i]);
    }
    printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}

/** procedure  print() : print array elements **/
void print() {
    int i;
    for (i = 0; i < N; i++) {
		if (i%8 == 0){
			printf("----\n");
		}
        printf("%d\n", array[i]);
    }
    printf("\n\n\n\n");
}

inline void exchange(int i, int j) {
    int t;
    t = array[i];
    array[i] = array[j];
    array[j] = t;
}
inline void compare(int i, int j, int dir) {
    if (dir==(array[i]>array[j]))
    exchange(i,j);
}

/**
* qsort() argument methods for sorting an array in each direction.
*/
int cmpfuncAsc (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}
int cmpfuncDes (const void * a, const void * b)
{
    return ( *(int*)b - *(int*)a );
}
