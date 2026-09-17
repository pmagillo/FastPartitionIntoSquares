#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "squares.h"

/* See squares.h for explaining comments. */

void SquareList_init(struct SquareList *L)
{ 
  L->first = L->last = NULL;
  L->length = 0;
}

void SquareList_append(struct SquareList *L, int x, int y, int s)
{
  struct SquareNode *N =
     (struct SquareNode *) malloc(sizeof(struct SquareNode));
  N->x0 = x; N->y0 = y; N->side = s; N->next = NULL;
  if (L->first==NULL) L->first = N;
  else L->last->next = N;
  L->last = N;
  L->length ++;
}

void SquareList_extend(struct SquareList *L, struct SquareList *T)
{
  struct SquareNode *N;
  while (T->first != NULL)
  {
    N = T->first;
    T->first = T->first->next;
    if (L->first==NULL) L->first = N;
    else L->last->next = N;
    L->last = N;
  }
  T->last = NULL;
  L->length += T->length;
  T->length = 0;
}

void printSquares(struct SquareList *L, char *file_name)
{
  FILE * F = fopen(file_name,"w");
  struct SquareNode *N = L->first;
  while (N!=NULL)
  {
    fprintf(F, "%d %d %d\n", N->x0, N->y0, N->side);
    N = N->next;
  }
  fclose(F);
}

int SquareList_len(struct SquareList *L)
{
  return L->length;
}


void SquareList_takefirst(struct SquareList *L, int *x, int *y, int *s)
{
  struct SquareNode *N = L->first;
  *x = N->x0; *y = N->y0;
  *s = N->side;
  L->first = L->first->next;
  free(N);
  L->length --;
}

int SquareList_full(struct SquareList *L) 
{
  return (L->first != NULL) ? 1 : 0;
}

void SquareList_copy(struct SquareList *srcL, struct SquareList *dstL)
{
  /* dispose old memory */
  struct SquareNode *N;
  while (dstL->first != NULL)
  {
    N = dstL->first;
    dstL->first = N->next;
    free(N);
  }
  /* copy from source to dest */
  dstL->first = srcL->first;
  dstL->last = srcL->last;
  dstL->length = srcL->length;
}

void SquareList_rotate(struct SquareList *L,
     int dimX, int dimY, int rotation)
{
  if (rotation==0) return;
  struct SquareNode *N = L->first;;
  int temp;
  while (N != NULL)
  {  
    if (rotation==1)
    {
      temp = N->x0+N->side;
      N->x0 = N->y0;
      N->y0 = dimY-temp;
    }
    else if (rotation==2)
    {
      N->x0 = dimX-(N->x0+N->side); 
      N->y0 = dimY-(N->y0+N->side);
    }
    else /* ==3 */
    {
      temp = N->y0+N->side;
      N->y0 = N->x0;
      N->x0 = dimX-temp;
    }
    N = N->next;
  }
}

int SquareList_max_side(struct SquareList *L)
{
  int maximum_side = 0;
  struct SquareNode *N = L->first;;
  while (N != NULL)
  {  
    if (N->side > maximum_side) maximum_side = N->side;
    N = N->next;
  }
  return maximum_side;
}
