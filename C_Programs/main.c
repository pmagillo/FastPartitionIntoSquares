#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "image2D.h"
#include "squares.h"
#include <time.h>

/*
Function makeSquares is defined in all the source files, each containing
a different decomposition algorithm. 
The main will be compiled together with one of them.
*/
extern void makeSquares(struct Image2D *img, struct SquareList *squares);

/*
Function to extract the filename from a given path.
*/
void take_name_from_path(char * path, char * name)
{
  int L = strlen(path), K = L;
  while (K>=0)
  {
    if (path[K]=='/') break;
    K--;
  }
  strcpy(name,path+K+1);
}

/*
Main function. If the source is compiled with -D EXEC_TIME, the execution
time of the decomposition process is measured and printed.
The expected parameters on the command-line are:
1: the name of the file containing the input black-and-white image
   encoded in binary format
2: optionally, the string NO to disable saving the decomposition
   (if the user wants just the execution time),
   or the name of the file where to save the decomposition
   (if no name is given, it will be csquares.txt).
*/
int main(int argc, char **argv)
{
  struct Image2D IMG;
  if (argc>1)
  {
    char aux[255];
    take_name_from_path(argv[1],aux);
    printf("%s: Reading image from %s\n",argv[0],aux);
    fprintf(stderr,"%s: Reading image from %s\n",argv[0],argv[1]);
    readBinary(&IMG, argv[1]);
  }
  else
  {
    printf("%s: Error, missing input_file_name\n",argv[0]);
    return 1;
  }
  printf("%s: Dimensions %d %d\n",argv[0],IMG.dimX,IMG.dimY);
  struct SquareList SQ;

#ifdef EXEC_TIME
  clock_t start_time, end_time;
  int spent_time;
  start_time = clock();
#endif 

  makeSquares(&IMG, &SQ);

#ifdef EXEC_TIME
  end_time = clock();
  spent_time = (end_time-start_time)*1000 / CLOCKS_PER_SEC;
/*  fprintf(stderr, "%s: Time (milliseconds): %d\n",argv[0], spent_time);*/
  printf("%s: Time (milliseconds): %d\n",argv[0], spent_time);
#endif

  printf("%s: Number of squares: %d\n",argv[0], SquareList_len(&SQ));
  printf("%s: Max square side: %d\n",argv[0], SquareList_max_side(&SQ));

  /* do not save the squares */
  if ((argc>2) && strcmp(argv[2],"NO")==0) return 0;
  
  /* save the squares */
  char out_name[250];
  if (argc>2) strcpy(out_name,argv[2]);
  else        strcpy(out_name,"csquares.txt");
  printSquares(&SQ, out_name);
  printf("%s: Squares printed to %s\n", argv[0], out_name);
  return 0;
}
