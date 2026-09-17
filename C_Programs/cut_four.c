/*
Decompose a binary image into a collection of disjoint squares
with any side length, by using a single scan of the image.
Version considering four rotated versions of the image,
and taking the one with the least number of squares.
*/

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "image2D.h"
#include "squares.h"

int biggestSquareFromRight(struct Image2D *img, int x0, int y0, int max_side)
{
  /*
  Given a foreground pixel of coordinates (x0,y0),
  find the side length of the biggest foreground square with (x0,y0) as its
  pixel with largest x, smallest y. */
  int side = 1;
  int dist = 2;
  /*printf("START biggestSquareFromRight %d %d %d\n",x0,y0, max_side);*/
  int chance = 1; /* true */
  while (chance && dist<=max_side)
  {
    /* vertical left side of square with edge=dist */
    for (int d=0; d<dist; d++)
    {
      if (Image2D_get(img, x0-dist+1,y0+d)==0)
      {  chance = 0;  break; }
    }
    if (!chance) break;
    /* horizontal bottom side of square with edge=dist */
    for (int d=1; d<dist; d++)
    {
      if (Image2D_get(img, x0-d+1,y0+dist-1)==0)
      {  chance = 0;  break; }
    }
    if (!chance) break;
    dist ++;
  }
  /*printf("biggestSquareFromRight (%d,%d) --> %d\n",x0,y0,(dist-1));*/
  return dist-1;
}

int biggestSquareFromLeft(struct Image2D *img, int x0, int y0, int max_side)
{
  /*
  Given a foreground pixel of coordinates (x0,y0),
  find the side length of the biggest foreground square with (x0,y0) as its
  pixel with smallest x, smallest y.
  */
  int side = 1;
  int dist = 2;
  /*printf("START biggestSquareFromLeft %d %d %d\n",x0,y0, max_side);*/
  int chance = 1; /* true */
  while (chance && dist<=max_side)
  {
    /* vertical left side of square with edge=dist */
    for (int d=0; d<dist; d++)
    {
      if (Image2D_get(img, x0+dist-1,y0+d)==0)
      {  chance = 0;  break; }
    }
    if (!chance) break;
    /* horizontal bottom side of square with edge=dist */
    for (int d=1; d<dist; d++)
    {
      if (Image2D_get(img, x0+d-1,y0+dist-1)==0)
      {  chance = 0;  break; }
    }
    if (!chance) break;
    dist ++;
  }
  /*printf("biggestSquareFromLeft (%d,%d) --> %d\n",x0,y0,(dist-1));*/
  return dist-1;
}


void makeSquaresAtRow(struct Image2D *img, int y, struct SquareList *squares)
{
  /*
  Create all the foreground squares with lowest coordinate =y and 
  delete them from  the image.
  The image pixels on all rows <y are all background.
  */
  /*printf("*********makeSquaresAtRow %d*****\n",y);*/
  /* Scan row y and find all the start and end pixels of a foreground run */
  SquareList_init(squares);
  int left_pix = -999, right_pix = -999;
  int run_length, side_from_left, side_from_right;
  int x0, side;
  for (int x=0; x<img->dimX; x++)
  {
  /*if (Image2D_get(img, x,y)==0) printf("  bacground pixel\n");*/
      if (Image2D_get(img, x,y)==0) continue;
      /* check if x is left corner */
      if (x==0 || Image2D_get(img, x-1,y)==0)
          left_pix = x;
      /* check if s is right corner */
      if (x==img->dimX-1 || Image2D_get(img, x+1,y)==0)
          right_pix = x;
      while (right_pix!=-999)
      {
          /* printf("  Run from %d to %d\n", left_pix,right_pix);*/
          /* A run ends at x, process the run.
          There is a cycle because the run may get shortened several times. */
          run_length = right_pix-left_pix+1;
          if (run_length==1)
          {
            /* printf("Square with corner sx %d unit side\n", left_pix);*/
            SquareList_append(squares, left_pix,y, 1);
            Image2D_put(img, left_pix,y, 0);
            left_pix = right_pix = -999;
          }
          else
          {
            /* find square to be created */
            side_from_left = biggestSquareFromLeft(img, left_pix,y, run_length);
            side_from_right = biggestSquareFromRight(img, right_pix,y, run_length);
            if (side_from_left>=side_from_right)
            {   x0 = left_pix; side = side_from_left;  }
            else
            {   x0 = right_pix-side_from_right+1; side = side_from_right;  }
            /* save square */
            /* printf("Square with corner sx %d side %d\n", x0,side);*/
            SquareList_append(squares, x0,y, side);
            /* delete square from image */
            for (int dx=x0; dx<x0+side; dx++)
            for (int dy=y; dy<y+side; dy++)
            {
                /*printf("   pixel %d %d becomes zero\n",dx,dy);*/
                Image2D_put(img, dx,dy, 0);
            }
            /* update run */
            if (side==run_length) /* run disappears */
              left_pix = right_pix = -999;
            else /*  run becomes shorter */
            {
              if (x0==left_pix) left_pix += side;
              else             right_pix -= side;
            }
            /* if (left_pix>=0) printf("  Reduced run %d %d\n",left_pix,right_pix);*/
        } /* end while: The run has been completely processed */
      }
  } /* end for: The row has been completely processed */
}


void makeSquaresOneAttempt(struct Image2D *img, struct SquareList *squares)
{
  /*
  Decompose the foreground object contained in the given image into squares. 
  As side effect, the image at the end will be all background.
  Return the list of squares in which the object has been decomposed.
  */
  SquareList_init(squares);
  /* find squares with upper corners on rows, excluding img->dimY-1 */
  for (int y=0; y<img->dimY-1; y++)
  {
    struct SquareList aux;
    makeSquaresAtRow(img, y, &aux);
    SquareList_extend(squares, &aux);
  }
  /* all remaining pixels on row img->dimY-1 are singleton squares */
  for (int x=0; x<img->dimX; x++)
  {
    if (Image2D_get(img, x,img->dimY-1)>0)
       SquareList_append(squares, x,img->dimY-1, 1);
  }
}

void makeSquaresOLD(struct Image2D *img, struct SquareList *squares)
{
  /*
  Consider the four rotations of the image
  */
  struct Image2D copied;
  struct SquareList sq;
  for (int rotation=3; rotation>=0; rotation--)
  {
    if (rotation>0) copyImage(img, &copied, rotation);
    if (rotation==3) makeSquaresOneAttempt(&copied, squares);
    else 
    {
      if (rotation==0) makeSquaresOneAttempt(img, &sq);
      else makeSquaresOneAttempt(&copied, &sq);
      if (SquareList_len(&sq)<SquareList_len(squares))
         SquareList_copy(&sq, squares);
    }
  }
}

void makeSquares(struct Image2D *img, struct SquareList *squares)
{
  /*
  Consider the four rotations of the image
  */
  struct Image2D copied;
  struct SquareList sq;
  int optimal_rotation = -1;
  SquareList_init(squares);
  for (int rotation=3; rotation>=0; rotation--)
  {
    /*printf("Rotation= %d\n",rotation);*/
    /* last time rotation is not necessary */
    if (rotation>0)
    {
       copyImage(img, &copied, rotation);
       makeSquaresOneAttempt(&copied, &sq);
    }
    else makeSquaresOneAttempt(img, &sq);
    /* if necessary, update final squares */
    if ((rotation==3) || (SquareList_len(&sq)<SquareList_len(squares)))
    {
       /*printf("update, opt rot now %d with %d squares\n",rotation, SquareList_len(&sq));*/
       optimal_rotation = rotation;
       SquareList_copy(&sq, squares);
    }
  }
  /* rotate back the final squares */
  if (optimal_rotation>0)
    SquareList_rotate(squares, img->dimX, img->dimY, optimal_rotation);
}
