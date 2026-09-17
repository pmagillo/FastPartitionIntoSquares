#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "pixelist.h"

/* See pixelist.h for explaining comments. */

void PixelList_init(struct PixelList *L) { L->first = L->last = NULL; }

void PixelList_append(struct PixelList *L, int x, int y)
{
  struct PixelNode *N = (struct PixelNode *) malloc(sizeof(struct PixelNode));
  N->x = x; N->y = y; N->next = NULL;
  if (L->first==NULL) L->first = N;
  else L->last->next = N;
  L->last = N;
}

void printPixels(struct PixelList *L)
{
  struct PixelNode *N = L->first;
  while (N!=NULL)
  {
    printf("(%d,%d) ", N->x, N->y);
    N = N->next;
  }
  printf("\n");
}

void PixelList_takefirst(struct PixelList *L, int *x, int *y)
{
  struct PixelNode *N = L->first;
  *x = N->x; *y = N->y;
  L->first = L->first->next;
  free(N);
}

int PixelList_full(struct PixelList *L) 
{
  return (L->first != NULL) ? 1 : 0;
}

void PixelList_remove(struct PixelList *L, int x, int y)
{
  struct PixelNode *N = L->first;
  struct PixelNode *M = NULL, T;
  while (N!=NULL)
  {
    if (N->x==x && N->y==y)
    {
      if (M!=NULL) M->next = N->next;
      else L->first = N->next;
      if (N==L->last) L->last = M;
      free(N);
      break;
    }
    M = N; 
    N = N->next;
  }
}

int PixelList_contains(struct PixelList *L, int x, int y)
{
  struct PixelNode *N = L->first;
  while (N!=NULL)
  {
    if (N->x==x && N->y==y) return 1;
    N = N->next;
  }
  return 0;
}

