/*
Decompose a binary image into a collection of disjoint squares
with arbitrary side length,
by using two different distance transforms.
*/

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "image2D.h"
#include "squares.h"
#include "pixelist.h"

/*
Given a binary image, create two new images where each background pixel of img 
has value==0 and each foreground pixel of img has 
- DMM value==its distance from the nearest background pixel having higher x and/or y
- Dmm value==its distance from the nearest background pixel having lower x and/or y
*/
void distanceTransformTwoScans(struct Image2D *img,
     struct Image2D *DMM, struct Image2D *Dmm,
     int * max_dist, struct PixelList ** dist_pixels)
{
  Image2D_init(DMM, img->dimX, img->dimY);
  Image2D_init(Dmm, img->dimX, img->dimY);
  *max_dist = 0;
  int distance, D, key;
  /* first scan */
  for (int y=0; y<img->dimY; y++)
  for (int x=0; x<img->dimX; x++)
  {
     if (Image2D_get(img, x,y)>0) /* foreground pixel */
     {
        if (x==0 || y==0)
          distance = 0;
        else
        {
          distance = Image2D_get(Dmm, x-1,y);
          D = Image2D_get(Dmm, x-1,y-1);
          if (D<distance) distance = D;
          D = Image2D_get(Dmm, x,y-1);
          if (D<distance) distance = D;
        }
        distance++;
        Image2D_put(Dmm, x,y, distance);
        if (distance>*max_dist) *max_dist = distance;
     }
  }
  /* second scan */
  int DIM = img->dimY;
  if (img->dimX<img->dimY) DIM = img->dimX;
  *dist_pixels = (struct PixelList*) malloc(DIM*sizeof(struct PixelList));
  for (int i=0; i<DIM; i++) PixelList_init(&(*dist_pixels)[i]);
  for (int y=img->dimY-1; y>=0; y--)
  for (int x=img->dimX-1; x>=0; x--)
  {
      if (Image2D_get(img, x,y)>0) /* foreground pixel */
      {
         if (x==img->dimX-1 || y==img->dimY-1)
           distance = 0;
         else
         {
           distance = Image2D_get(DMM, x+1,y);
           D = Image2D_get(DMM, x+1,y+1);
           if (D<distance) distance = D;
           D = Image2D_get(DMM, x,y+1);
           if (D<distance) distance = D;
         }
         distance++;
         Image2D_put(DMM, x,y, distance);
         if (distance>*max_dist) *max_dist = distance;
         key = distance;
         D = Image2D_get(Dmm, x,y);
         if (D>key) key = D;
         PixelList_append(&(*dist_pixels)[key], x,y);
      } 
  }
}

/*
Update the value of pixel (x,y) by taking the 1 + minimum of its neighbors.
*/
void updateDMMDistances(struct Image2D *DMM, struct Image2D *Dmm, 
     struct PixelList *pixels, struct PixelList *updated_pixels)
{
  PixelList_init(updated_pixels);  
  int x,y, old_dist, new_dist, D;
  while (PixelList_full(pixels))
  {
    PixelList_takefirst(pixels, &x, &y);
    old_dist = Image2D_get(DMM, x,y);
    if (old_dist==0)  continue;
    new_dist = Image2D_get(DMM, x,y+1);
    D = Image2D_get(DMM, x+1,y+1);
    if (D<new_dist) new_dist = D;
    D = Image2D_get(DMM, x+1,y);
    if (D<new_dist) new_dist = D;
    new_dist++;
    if (new_dist<old_dist)
    {
       Image2D_put(DMM, x,y, new_dist);
       PixelList_append(pixels, x,y-1);
       PixelList_append(pixels, x-1,y-1);
       PixelList_append(pixels, x-1,y);
       if (old_dist>Image2D_get(Dmm, x,y)) PixelList_append(updated_pixels, x,y);
    }
  }
}

void updateDmmDistances(struct Image2D *DMM, struct Image2D *Dmm, 
     struct PixelList *pixels, struct PixelList *updated_pixels)
{
  PixelList_init(updated_pixels);  
  int x,y, old_dist, new_dist, D;
  while (PixelList_full(pixels))
  {
    PixelList_takefirst(pixels, &x, &y);
    old_dist = Image2D_get(Dmm, x,y);
    if (old_dist==0)  continue;
    new_dist = Image2D_get(Dmm, x,y-1);
    D = Image2D_get(Dmm, x-1,y-1);
    if (D<new_dist) new_dist = D;
    D = Image2D_get(Dmm, x-1,y);
    if (D<new_dist) new_dist = D;
    new_dist++;
    if (new_dist<old_dist)
    {
       Image2D_put(Dmm, x,y, new_dist);
       PixelList_append(pixels, x,y+1);
       PixelList_append(pixels, x+1,y+1);
       PixelList_append(pixels, x+1,y);
       if (old_dist>Image2D_get(DMM, x,y)) PixelList_append(updated_pixels, x,y);
    }
  }
}


void defineSquaresMaxSide(struct Image2D *DMM, struct Image2D * Dmm, 
     struct PixelList ** dist_pixels, int max_dist, struct SquareList *squares)
{
  SquareList_init(squares);
  /* Scan to find pixels with max distance in DMM or Dmm */
  int x,y, D, d, x0, y0, key, xx,yy;
  struct PixelList max_pixels, pixels, updated;
  PixelList_init(&max_pixels);
  /*not necessary: if ((*dist_pixels)[max_dist]->first == NULL) return; */

  while (PixelList_full( &(*dist_pixels)[max_dist] ))
  {
    PixelList_takefirst( &(*dist_pixels)[max_dist], &x, &y);
    D = Image2D_get(DMM, x,y);
    d = Image2D_get(Dmm, x,y);
    if (D==max_dist || d==max_dist) PixelList_append(&max_pixels, x,y);
  }
  /* now max_pixels contains all pixels having that distance */
  while (PixelList_full(&max_pixels))
  {
    PixelList_takefirst(&max_pixels, &x, &y);
    /* Form the square of side max_dist */
    D = Image2D_get(DMM, x,y);
    d = Image2D_get(Dmm, x,y);
    if (max_dist==D)
    {
      x0 = x; y0 = y;
    }
    else if (max_dist==d)
     {
      x0 = x-max_dist+1; y0 = y-max_dist+1;
    }
    else
    {
      key = (D > d) ? D : d;
      if (key>0) PixelList_append(&(*dist_pixels)[key], x,y);
      continue;
    }
    /* Create the square and delete the square from the images */
    SquareList_append(squares, x0,y0, max_dist);
    for (int dx=x0; dx<x0+max_dist; dx++)
    for (int dy=y0; dy<y0+max_dist; dy++)
    {
      Image2D_put(DMM, dx,dy, 0);
      Image2D_put(Dmm, dx,dy, 0);
    }
    /* Update distances. No need to update if we are making singleton squares */
    if (max_dist==1)   continue;
    /* DMM must be updated on the square sides <x0, <y0 */
    PixelList_init(&pixels);
    for (int dy=y0-1; dy<y0+max_dist; dy++)
      if (Image2D_get(DMM, x0-1,dy)>0)  
         PixelList_append(&pixels, x0-1,dy);
    for (int dx=x0-1; dx<x0+max_dist; dx++)
      if (Image2D_get(DMM, dx,y0-1)>0)
         PixelList_append(&pixels, dx,y0-1);
    updateDMMDistances(DMM, Dmm, &pixels, &updated);
    while (PixelList_full(&updated))
    {
       PixelList_takefirst(&updated, &xx, &yy);
       key = Image2D_get(DMM, xx,yy);
       d = Image2D_get(Dmm, xx,yy);
       if (d>key) key = d;
       PixelList_append(&(*dist_pixels)[key], xx,yy);
    }
    /* Dmm must be updated on the square sides >=x0+D, >=y0+D */
    PixelList_init(&pixels);
    for (int dy=y0; dy<y0+max_dist+1; dy++)
      if (Image2D_get(DMM, x0+max_dist,dy)>0)
         PixelList_append(&pixels, x0+max_dist,dy);
    for (int dx=x0; dx<x0+max_dist+1; dx++)
      if (Image2D_get(DMM, dx,y0+max_dist)>0)
         PixelList_append(&pixels, dx,y0+max_dist);
    updateDmmDistances(DMM, Dmm, &pixels, &updated);
    while (PixelList_full(&updated))
    {
       PixelList_takefirst(&updated, &xx, &yy);
       key = Image2D_get(DMM, xx,yy);
       d = Image2D_get(Dmm, xx,yy);
       if (d>key) key = d;
       PixelList_append(&(*dist_pixels)[key], xx,yy);
    }
  }
}

void makeSquares(struct Image2D *img, struct SquareList *squares)
{
  struct Image2D DMM, Dmm;
  int max_dist;
  struct PixelList * pixel_dict;
  struct SquareList new_squares;
  distanceTransformTwoScans(img, &DMM, &Dmm, &max_dist, &pixel_dict);
  SquareList_init(squares);
  while (max_dist>0)
  {
    defineSquaresMaxSide(&DMM, &Dmm, &pixel_dict, max_dist, &new_squares);
    SquareList_extend(squares, &new_squares);
    max_dist --;
  }
}
