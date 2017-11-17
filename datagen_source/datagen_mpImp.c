/**
* FILE: mpImp.c
* THMMY, 7th semester, Parallel and Distributed Systems: 1st assignment
* Parallel implementation of bitonic sort
* Authors:
*   Kamtziridis Georgios, 8542, gkamtzir@auth.gr
*   Katomeris Nikolaos  , 8551, ngkatomer@auth.gr
* Compile command with gcc example:
*   gcc mpImp.c -O3 -fopenmp -o <executable file name>
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
#include <omp.h>

struct timeval startwtime, endwtime;
double seq_time;

//direction macro definitions
#define ASCENDING 1
#define DESCENDING 0

//GLOBAL VARIABLES FOR EASY ACCESS FROM ALL FUNCTIONS
int nt;
int N;
int *array;
int CALLED_ARRAY_SIZE;
int MAX_ARRAY_SIZE;

//FUNCTION DECLARATIONS
void impBitonicSort();
void customQuickSort(int start_index, int cnt, int direction);
int cmpfuncAsc (const void * a, const void * b);
int cmpfuncDes (const void * a, const void * b);
/**
* Runs a function and returns its execution time.
* The 3rd and 4th arguments are for choosing to print the result as well
* (<printResultFlag> != 0) with the name of the function <fname>
*/
double timeRun(void (*function()), int printResultFlag, char* const fname,
            int argumentsFlag, int start, int cnt, int dir);

//Original file bitonic.c functions
void init(void);
void print(void);
void test(void);
inline void exchange(int i, int j);
inline void compare(int i, int j, int dir);

/** the main program **/ 
int main(int argc, char **argv) {

	//Check main arguments and pass them to the global variables
	if (argc != 4) {
		printf("Usage: %s n q d\nWhere 2^n is the number of the array elements"
			  " and 2^q the number of threads\n. d is the MAX_ARRAY_SIZE\n"
			  "This program should be called from a script\n", argv[0]);
		exit(1);
	}
	int n = atoi(argv[1]);
	int q = atoi(argv[2]);

	N = 1<<n;
	nt = 1<<q;
	CALLED_ARRAY_SIZE = n; 
	MAX_ARRAY_SIZE = atoi(argv[3]);

	array = (int *) malloc(N * sizeof(int));	
	if (array == NULL) {
		const int MB_size = 8*1024*1024; //1 MB = 1024 KB = 1024 B = 8 bit
		printf("Couldn't allocate memory for the array, probably because of"
				" its size: %fMB\n", (float) N*sizeof(int)/MB_size);
		exit(-1);
	}

	//Forcing OpenMP to use the exact number of threads
	//provided
	omp_set_dynamic(0);
	//Setting the exact number of threads
	omp_set_num_threads(nt);  

	//Sort the array with parallel bitonic sort, and time the sort.
	//Check for correctness of the sort.
	init();
	timeRun((void*)impBitonicSort, 1, "Imperative openMP", 0, 0, 0, 0);

	//test();

	//Re-randomize the array and sort it with quicksort, print the sorting time.
	//init(); 
	//timeRun((void*)customQuickSort, 1, "Quicksort", 1, 0, N, ASCENDING);

	//Free the allocated memory from the heap.
	free(array);

	//Return 0 indicating successful terminatioin of the program.
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

/*
  imperative version of bitonic sort
*/
void impBitonicSort() {

	int i,j,k;
	int L = N / 8; 

	int u;
	#pragma omp parallel for num_threads(nt)
	for (u = 0; u < N/L; u++){

		if(u%2) {
			customQuickSort(u * L, L, DESCENDING);
		} else {
			customQuickSort(u * L, L, ASCENDING);
		}

	}
		


	for (k=2*L; k<=N; k=k*2){    
		for (j=k>>1; j>0; j=j>>1) {
			#pragma omp parallel for num_threads(nt)
			for (i=0; i<N; i++) {
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
* Runs a function and returns its execution time.
* The 3rd and 4th arguments are for choosing to print the result as well
* (<printResultFlag> != 0) with the name of the function <fname>
*/
double timeRun(void (*function()), int printResultFlag, char* const fname,
               int argumentsFlag, int start, int cnt, int dir)
{
    struct timeval startwtime, endwtime;
    
    if (argumentsFlag)
    {
      gettimeofday (&startwtime, NULL);
      function(start, cnt, dir);
      gettimeofday (&endwtime, NULL);
    } else
    {
      gettimeofday (&startwtime, NULL);
      function();
      gettimeofday (&endwtime, NULL);
    }    

    double exec_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
            + endwtime.tv_sec - startwtime.tv_sec);
    
    if (printResultFlag && CALLED_ARRAY_SIZE != MAX_ARRAY_SIZE)
        printf("%lf, ", exec_time);
    else 
		printf("%lf", exec_time);
	
    return exec_time;
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
