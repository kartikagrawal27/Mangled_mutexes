/**
 * Mangled Mutexes Lab
 * CS 241 - Spring 2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

/* You should create a struct that will get passed in by reference to your
 * start_routine. */

struct temparray{
  int tempstart;
  int tempend;
};

typedef struct temparray myarray;
// Globals
size_t number_of_primes; // global number of primes you need to update
/* You should create a mutex for said global variable. */
pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
// ID structs for the threads



/* You should create a start routine for your threads. */

// This function parses the arguments in argv.
// An error message is printed if any of the following happens:
//  An incorrect number of arguments are passed in.
//  'start', 'end', or 'num_threads' is less than 1.
//  'end' < 'start'.
// Else an array of 3 size_t elements are returned
// in the following order: 'start', 'end', 'num_threads'.
// It is the callers job to free the memory used.
size_t *arg_parse(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "usage: %s [start] [end] [num_threads]\n", argv[0]);
    exit(1);
  }

  char *endptr;
  long tokargs[3];
  for (size_t i = 0; i < sizeof(tokargs) / sizeof(tokargs[0]); i++) {
    tokargs[i] = strtol(argv[i + 1], &endptr, 10);
    if (*endptr != '\0') {
      fprintf(stderr, "Failed to convert an arugment to a long!\n");
      exit(2);
    }
    if (tokargs[i] < 1) {
      fprintf(stderr,
              "Please have all arguments be greater than or equal to 1!\n");
      exit(3);
    }
  }
  if (tokargs[1] < tokargs[0]) {
    fprintf(stderr, "Please give a valid range!\n");
    exit(4);
  }
  size_t *args = malloc(3 * sizeof(size_t));
  for (size_t i = 0; i < sizeof(tokargs) / sizeof(tokargs[0]); i++) {
    args[i] = (size_t)tokargs[i];
  }
  return args;
}

// Call on this function when you are ready to print the result.
void print_result(size_t number_of_primes, size_t start, size_t end) {
  printf("There are %zu primes between %zu and %zu inclusive\n",
         number_of_primes, start, end);
}

int primechecker(int tocheck)
{  
  if(tocheck==2)
    return 1;
  if(tocheck==1)
      return 0;

   int c;
   for( c = 2;c <= sqrt(tocheck);c++ )
   { 
      if (tocheck%c==0)
      return 0;
   }
  return 1;
}

void *primehelper(void * myint)
{  
  myarray * temp = myint;
  int tempstart = temp->tempstart;
  int tempend = temp->tempend;

  for(int i= tempstart;i<=tempend;i++)
  {
    if(primechecker(i)==1)
    {
      //printf("the value is %d:\n", i);
      pthread_mutex_lock(&mylock);
        number_of_primes++;
      pthread_mutex_unlock(&mylock);
    }
  }
  return NULL;
}

// Prints the number of primes
// inbetween argv[1] and argv[2] inclusive with argv[3] threads.
int main(int argc, char *argv[]) {
  /* Your implementation goes here */

  size_t * temparray = arg_parse(argc, argv);
  int numthreads, mystart, myend;
  mystart = temparray[0];
  myend = temparray[1];

  numthreads = temparray[2];

  if(numthreads>(myend-mystart+1))
  {
    numthreads = myend-mystart+1;
  }

  pthread_t threadid[numthreads];

  number_of_primes=0;

  myarray *computer = malloc(sizeof(myarray)*numthreads);

  int num_ele_pthread  = ((myend-mystart)/numthreads);

  for(int i=0;i<numthreads;i++)
  {
    if(i==0)
    {
      computer[i].tempstart = mystart;
      computer[i].tempend = mystart+num_ele_pthread;
    }
    else if(i==numthreads-1)
    {
      computer[i].tempstart = computer[i-1].tempend+1;
      computer[i].tempend = myend;
    }
    else
    {
      computer[i].tempstart = (computer[i-1].tempend)+1;
      computer[i].tempend = computer[i].tempstart + num_ele_pthread;
    }

    pthread_create(&threadid[i], NULL, primehelper, &computer[i]);

  }

  for(int i=0;i<numthreads;i++)
  {
    pthread_join(threadid[i], NULL);
  }

  print_result(number_of_primes, mystart, myend);

  return 0;
}