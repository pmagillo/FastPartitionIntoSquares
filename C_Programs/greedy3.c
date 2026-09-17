/*
Decompose a binary image into a collection of disjoint squares
with arbitrary side length, by using a greedy scan.
*/

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "image2D.h"
#include "squares.h"
#include "pixelist.h"

/*
Initialize an image (nodes) where nodes[x][y] contains the side length 
of the square having minimum coordinate corner at (x,y);
if no such square is in the graph, it stores 0.
At the beginning nodes contains a square of side length 1 for
each foreground pixel of the input image img.
*/
void createImageGraph(struct Image2D *nodes, struct Image2D *img)
{
  Image2D_init(nodes, img->dimX, img->dimY);
  for (int y=0; y<img->dimY; y++)
  for (int x=0; x<img->dimX; x++)
  {
     if (Image2D_get(img, x,y)>0) /* foreground pixel */
     {
         Image2D_put(nodes, x,y, 1);
     }
   } 
}

/*
Unify the nodes having minimum corners at the coordinates in old_nodes, 
into a single node with minimum corner at pixel (x0,y0) with edge length equal 
to edge.
The union of all the unified nodes must coincide with that square
(the function does not check).
*/
void unify_nodes(struct Image2D *nodes,
     struct PixelList *united,  int x0, int y0, int edge)
{
  int x,y;
  while(PixelList_full(united))
  {
    PixelList_takefirst(united, &x, &y);
    Image2D_put(nodes, x,y, 0);
  }
  /* new value for node at x0,y0 */
  Image2D_put(nodes, x0, y0, edge);
}


/*
Check if the big square with minimum corner in (x0,y0) and maximum corner
in (x1,y1), with edge=target, is covered by squares of nodes.
If yes, the list united will contain the nodes forming the union. 
*/
int canMakeSquare(struct Image2D *nodes, 
    int x0, int y0, int x1, int y1, 
    struct PixelList *united, int target)
{
  /*printf("canMakeSquare %d %d -- %d %d target edge %d\n",x0,y0,x1,y1,target);*/
  PixelList_init(united);
  target *= target; /* target was edge length, make it area */
  int area = 0;
  int x,y, L;
  struct PixelList quad_queue;
  PixelList_init(&quad_queue);
  PixelList_append(&quad_queue, x0,y0);
  while (PixelList_full(&quad_queue))
  {
    PixelList_takefirst(&quad_queue, &x, &y);
    L = Image2D_get(nodes, x,y);
    /*if ((x+L>x1+1)||(y+L>y1+1)) printf("  quad %d %d side %d is outside\n",x,y,L);*/
    if ( (x+L>x1+1) || (y+L>y1+1) )
    {
       /* some square is partially outside the big target square: no chance */
       /* empty the list */
       while (PixelList_full(&quad_queue))
         PixelList_takefirst(&quad_queue, &x, &y);
       /* set up area so that the function will return false; */
       area = 0;
       break;
    }
    if (L>0)
    {
       /*printf(" united %d %d \n",x,y);*/
       PixelList_append(united, x,y);
       area += (L*L);
       Image2D_put(nodes, x,y, -L);/*  to avoid considering it twice */
       if (x+L<=x1) PixelList_append(&quad_queue, x+L,y);
       if (y+L<=y1) PixelList_append(&quad_queue, x,y+L);
    }
  }
  if (area==target) return 1; /* true */
  /* else restore values */
  while (PixelList_full(united))
  {
     PixelList_takefirst(united, &x, &y);
     Image2D_put(nodes, x,y, -Image2D_get(nodes, x,y));
  }
  return 0; /* false */
}

/*
Check if it is possible extend a square from (x1,y1) in direction 
of -y, -x to a larger edge length than 1.
If yes, extend the square. 
*/
void extension(struct Image2D *nodes, int x1, int y1)
{
  /*printf("START extension %d %d\n", x1,y1);*/
  int x0=x1-1, y0 = y1-1;
  int L, bigL;
  struct PixelList united;
  while ((x0>=0) && (y0>=0))
  {
    L = Image2D_get(nodes, x0,y0);
    if (L>0)
    {
      bigL = x1-x0+1;
      /*printf("Can I merge? %d %d -- %d %d into square of side %d\n", x0,y0, x1,y1, bigL);*/
      if ( canMakeSquare(nodes, x0,y0, x1,y1, &united, bigL) )
          unify_nodes(nodes, &united, x0,y0, bigL);
      else
          break;
    }
    x0--;
    y0--;
  }
}

void makeSquares(struct Image2D *img, struct SquareList *squares)
{
  struct Image2D nodes;
  int L;
  /* Create singleton squares */
  createImageGraph(&nodes, img);
  /* Scan to form squares of side length > 1 */
  for (int y=1; y<img->dimY; y++)
  for (int x=1; x<img->dimX; x++)
  {
    if (Image2D_get(&nodes, x,y)>0)
          extension(&nodes,  x,y);
  }
  /* Take squares out of the graph */
  SquareList_init(squares);
  for (int y=0; y<img->dimY; y++)
  for (int x=0; x<img->dimX; x++)
  {
    L = Image2D_get(&nodes, x,y);
    if (L>0) SquareList_append(squares, x,y, L);
  }
}
