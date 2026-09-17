/*
The quadtree encodes a 2D image of size LxL where
L = 2^E for some natural E.
The pixel of minimum coordinates in the image is (0,0) 
and the pixel with largest coordinates is (L-1,L-1).
The quadtree stores:
- the lengths of two sides of the original image along x,y
- side of the square domain of the quadtree (power of two)
- set of leaves implemented as a dictionary with key the location code
Each node (representing a square) stores:
- the coordinates of an anchor point (xmin,ymin)
- side of the square domain of the node
The color is not stored because we make just foreground nodes.
*/

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "image2D.h"
#include "squares.h"


/* Return the next of (x,y) in the Z order. */
void nextZorder(int x, int y, int *x1, int *y1)
{
  if (x%2==0) { *x1 = x+1; *y1 = y; return; }
  if (y%2==0) { *x1 = x-1; *y1 = y+1; return; }
  int xx,yy;
  nextZorder(x/2,y/2, &xx, &yy);
  *x1 = 2*xx; *y1 = 2*yy;
}


/*
Return the exponent E of the minimum power of two
which is greater then or equal to L1 and L2.
L1, L2 must be positive integers (they are the
lengths of the three sides of the image).
*/ 
int power_of_two(int L1, int L2)
{
  int L = 1, E = 0;
  while (L<L1 || L<L2)
  {
    L *= 2; E += 1;
  }
  return L;
}

/* Build the quadtree for the given 2D image. */
void makeSquares(struct Image2D *IMG, struct SquareList *squares)
{
  int Qside = power_of_two(IMG->dimX, IMG->dimY);
  SquareList_init(squares);
  struct SquareList prev_squares, next_squares;
  SquareList_init(&prev_squares);
  SquareList_init(&next_squares);
  /* Build nodes at first level */
  /* printf("Build squares at level 1\n");*/
  int todo = Qside*Qside;
  int x = 0, y = 0;
  int x1,y1, x2,y2, x3,y3;
  int messi = 0, finali = 0;
  while (todo>0)
  {
    nextZorder(x, y,  &x1,&y1);
    nextZorder(x1,y1, &x2,&y2);
    nextZorder(x2,y2, &x3,&y3);
    /* printf("Can I merge? (%d %d), (%d %d), (%d %d), (%d %d)\n",x,y, x1,y1, x2,y2, x3,y3);
       printf(" colors %d %d %d %d\n", Image2D_get(IMG,x,y),Image2D_get(IMG,x1,y1),Image2D_get(IMG,x2,y2),Image2D_get(IMG,x3,y3));*/
    if (Image2D_get(IMG,x,y)>0 && Image2D_get(IMG,x1,y1)>0 && 
        Image2D_get(IMG,x2,y2)>0 && Image2D_get(IMG,x3,y3)>0)
    {
      /*      printf("...yes, merge\n");*/
      SquareList_append(&next_squares, x,y, 2);
      messi ++;
    }
    else
    {
      /*      printf("...no, do not merge\n");*/
      if (Image2D_get(IMG,x,y)>0)   {SquareList_append(squares, x,y, 1); finali++;}
      if (Image2D_get(IMG,x1,y1)>0) {SquareList_append(squares, x1,y1, 1); finali++;}
      if (Image2D_get(IMG,x2,y2)>0) {SquareList_append(squares, x2,y2, 1); finali++;}
      if (Image2D_get(IMG,x3,y3)>0) {SquareList_append(squares, x3,y3, 1); finali++;}
    }
    nextZorder(x3,y3, &x,&y);
    todo -= 4;
  }
  /*  printf("At level 1  %d final squares, %d next squares\n", finali, messi);*/

  /* Build nodes at next levels: they are already sorted in Z order */
  int chance = 1; /* true */
  int level = 2;
  while (chance) 
  {
    messi = finali = 0;
    /* printf("Build squares at level %d\n",level);*/
    prev_squares.first = next_squares.first; /* copy */
    prev_squares.last = next_squares.last;
    SquareList_init(&next_squares);
    struct SquareNode *N, *N1, *N2, *N3;
    chance = 0; /* false */
    while (prev_squares.first!=NULL)
    {
      N = prev_squares.first;
      prev_squares.first = prev_squares.first->next;
      /* printf("Can I merge?  N= %d %d (side %d)\n",N->x0,N->y0,(2*N->side));*/
      /* N1 must be x0+level, y0 */
      N1 = N->next;
      if (N1==NULL)
      { finali++; SquareList_append(squares, N->x0, N->y0, N->side); continue; }
      /* printf("N1= %d %d (side %d) but expected %d %d\n", N1->x0,N1->y0,N1->side,  N->x0+level,N->y0);*/
      if ((N1->x0 != N->x0+level) || (N1->y0 != N->y0))
      { finali++; SquareList_append(squares, N->x0, N->y0, N->side); continue; }
      /* N2 must be x0, y0+level */
      N2 = N1->next;
      if (N2==NULL)
      { finali++; SquareList_append(squares, N->x0, N->y0, N->side); continue; }
      /*      printf("N2= %d %d (side %d) but expected %d %d\n", N2->x0,N2->y0,N2->side,  N->x0,N->y0+level);*/
      if ((N2->x0 != N->x0) || (N2->y0 != N->y0+level))
      { finali++; SquareList_append(squares, N->x0, N->y0, N->side); continue; }
      /* N2 must be x0+level, y0+level */
      N3 = N2->next;
      if (N3==NULL)
      { finali++; SquareList_append(squares, N->x0, N->y0, N->side); continue; }
      /*      printf("N3= %d %d (side %d) but expected %d %d\n", N3->x0,N3->y0,N3->side,  N->x0+level,N->y0+level);*/
      if ((N3->x0 != N->x0+level) || (N3->y0 != N->y0+level))
      { finali++; SquareList_append(squares, N->x0, N->y0, N->side); continue; }
      /* we can merge */
      /* printf("Merge %d %d, %d %d, %d %d, %d %d\n",N->x0,N->y0, N1->x0,N1->y0, N2->x0,N2->y0, N3->x0,N3->y0);*/
      SquareList_append(&next_squares, N->x0, N->y0, 2*N->side);
      messi ++;
      chance = 1; /* true */
      /* remove other 3 elements (N1,N2,N3) from prev_list */
      prev_squares.first = prev_squares.first->next;
      prev_squares.first = prev_squares.first->next;
      prev_squares.first = prev_squares.first->next;
      /* printf("Next iteration level %d, first node=%p\n",level, prev_squares.first);*/
    }
    /* printf("End of level. Put %d messi %d final squares, %d next squares\n", level,finali, messi);*/
    level *= 2;
  }
  SquareList_extend(squares, &next_squares);
}
