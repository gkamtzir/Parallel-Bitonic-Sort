/**
* FILE: cilkImp.c
* THMMY, 7th semester, Parallel and Distributed Systems: 1st assignment
* Parallel implementation of bitonic sort
* Authors:
*   Kamtziridis Georgios, 8542, gkamtzir@auth.gr
*   Katomeris Nikolaos  , 8551, ngkatomer@auth.gr
* Compile command with gcc example:
*   gcc cilkImp.c -O3 -fcilkplus -o <executable file name>
* Usage: ./<executable file name> n q
*   where 2^n is the number of the array elements and 2^q the number of threads.
*   if q >= n, the program will run as if q = n - 1.
* Run command example:
*   ./a.out 24 3
* will sort an array with 2^24 elements using 2^3 threads almost simultaneously
* This program works with arrays up to 2^30 on machines where the integer size
* in memory is 4 bytes.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

struct timeval startwtime, endwtime;
double seq_time;

//GLOBAL VARIABLES FOR EASY ACCESS FROM ALL FUNCTIONS
int nt;
int N;
int *array;

//direction macro definitions
#define  ASCENDING  1
#define  DESCENDING 0

//FUNCTION DECLARATIONS
void impBitonicSort();
void customQuickSort(int start_index, int cnt, int direction);
int cmpfuncAsc (const void * a, const void * b);
int cmpfuncDes (const void * a, const void * b);
double timeRun(void (*function()), int printResultFlag, 
         char* const fname, int argumentsFlag, int start, int cnt, int dir);

//Original file bitonic.c functions
void init(void);
void print(void);
void test(void);
inline void exchange(int i, int j);
inline void compare(int i, int j, int dir);

/** the main program **/ 
int main(int argc, char **argv) {

	//Check main arguments and pass them to the global variables
	if (argc != 3) {
		printf("Usage: %s n q\nWhere 2^n is the number of the array elements"
			  " and 2^q the number of threads\n", argv[0]);
		exit(1);
	}

	N = 1<<atoi(argv[1]);
	nt = 1<<atoi(argv[2]);  

	//Setting the number of threads	
	char number_of_threads[12];
	sprintf(number_of_threads, "%d", nt);

	array = (int *) malloc(N * sizeof(int));
	if (array == NULL) {
		const int MB_size = 8*1024*1024; //1 MB = 1024 KB = 1024 B = 8 bit
		printf("Couldn't allocate memory for the array, probably because of"
				" its size: %fMB\n", (float) N*sizeof(int)/MB_size);
		exit(-1);
	}

	__cilkrts_set_param("nworkers", number_of_threads);  

	//Sort the array with parallel bitonic sort, and time the sort.
	//Check for correctness of the sort.
	init();
	timeRun((void*)impBitonicSort, 1, "Imperative cilk sort", 0, 0, 0, 0);
	test();

	//Re-randomize the array and sort it with quicksort, print the sorting time.
	init();
	timeRun((void*)customQuickSort, 1, "Quicksort", 1, 0, N, ASCENDING);

	//Free the allocated memory from the heap.
	free(array);

	//Return 0 indicating successful termination of the program.
	return 0;

}

/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (array[i-1] <= array[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}

/**
* Runs a function and returns its execution time.
* The 5th and 6th arguments are for choosing to print the result as well
* (<printResultFlag> != 0) with the name of the function <fname>
*/
double timeRun(void (*function()), int printResultFlag, 
         char* const fname, int argumentsFlag, int start, int cnt, int dir)
{
    struct timeval startwtime, endwtime;
	
    if (argumentsFlag)
    {
      	gettimeofday (&startwtime, NULL);
      	function(start, cnt, dir);
      	gettimeofday (&endwtime, NULL);
    } 
    else {
		gettimeofday (&startwtime, NULL);
        function();
        gettimeofday (&endwtime, NULL);
    }

    double exec_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
            + endwtime.tv_sec - startwtime.tv_sec);
    if (printResultFlag)
        printf("%s execution time = %lf\n", fname, exec_time);
    return exec_time;
}

/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    array[i] = rand() % N; // (N - i);
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", array[i]);
  }
  printf("\n");
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

/*
  Imperative bitonic sort that uses cilk.
*/
void impBitonicSort() {

	int i,j,k;
	int L = N / 8; 

	int u;
	cilk_for (u = 0; u < N/L; u++){

		if(u%2) {
			customQuickSort(u * L, L, 0);
		} else {
			customQuickSort(u * L, L, 1);
		}

	}

	for (k=2*L; k<=N; k=k*2){    
		for (j=k>>1; j>0; j=j>>1) {
			cilk_for (i=0; i<N; i++) {
				int ij=i^j;
				if ((ij)>i) {
					if ((i&k)==0 && array[i] > array[ij]) 
						exchange(i,ij);
					if ((i&k)!=0 && array[i] < array[ij])
						exchange(i,ij);
				}
			}
		}   
	}
}

void customQuickSort(int start_index, int cnt, int direction){
    if (direction == ASCENDING)
        qsort(&array[start_index], cnt, sizeof(int), cmpfuncAsc);
    else
        qsort(&array[start_index], cnt, sizeof(int), cmpfuncDes);
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
