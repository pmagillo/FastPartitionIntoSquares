/*
Decompose a binary image into a collection of disjoint squares
with arbitrary side length, by using the distance transform.
*/

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "image2D.h"
#include "squares.h"
#include "pixelist.h"

/*
Given an image img, create new image dist_img where 
each background pixel of img has value==0 and 
each foreground pixel of img has value==its distance from the nearest 
background pixel (computed as min between vertical distance
and horizontal distance).
Put into max_dist the maximum value of dist_img.
Create dist_pixels as an array of lists of pixels, where
dist_pixels[d] contains the pixel with value ==d in dist_img.
*/
void distanceTransformTwoScans(struct Image2D *img,
     struct Image2D *dist_img,
     int *max_dist, struct PixelList **dist_pixels)
{
  Image2D_init(dist_img, img->dimX, img->dimY);
  int min_distance, d;
  int DIM = img->dimY;
  if (img->dimX<img->dimY) DIM = img->dimX;
  DIM = (DIM+1)/2;
  
  /* first scan */
  for (int y=0; y<img->dimY; y++)
  for (int x=0; x<img->dimX; x++)
  {
      if (Image2D_get(img, x,y)>0) /* foreground pixel */
      {
        if (x==0 || y==0)
           min_distance = 0;
        else
        {
           int XX[4] = {x-1, x-1, x, x+1};
           int YY[4] = {y, y-1, y-1, y-1};
           min_distance = Image2D_get(dist_img, XX[0],YY[0]);
           for (int i=1; i<4; i++)
           {
             d = Image2D_get(dist_img, XX[i],YY[i]);
             if (d<min_distance) min_distance = d;
           }
        }
        Image2D_put(dist_img, x,y, min_distance+1);
      }
  }
  /* second scan */
  *max_dist = 0;  
  *dist_pixels = (struct PixelList*) malloc(DIM*sizeof(struct PixelList));
  for (int i=0; i<DIM; i++) PixelList_init(&(*dist_pixels)[i]);
  for (int y=img->dimY-1; y>=0; y--)
  for (int x=img->dimX-1; x>=0; x--)
  {
     /*printf("Check %d %d\n", x,y);*/
     if (Image2D_get(img, x,y)>0) /* foreground pixel */
     {
        /*printf("Check %d %d : ok\n", x,y);*/
        if (x==img->dimX-1 || y==img->dimY-1)
           min_distance = 0;
        else
        {
           int XX[4] = {x+1, x+1, x, x-1};
           int YY[4] = {y, y+1, y+1, y+1};
           min_distance = Image2D_get(dist_img, XX[0],YY[0]);
           for (int i=1; i<4; i++)
           {
             d = Image2D_get(dist_img, XX[i],YY[i]);
             if (d<min_distance) min_distance = d;
           }
        }
        min_distance++;
        d = Image2D_get(dist_img, x,y);
        if (min_distance<d)
        {  d = min_distance; Image2D_put(dist_img, x,y, d);  }
        if (d>*max_dist) *max_dist = d;
        PixelList_append(&(*dist_pixels)[d], x,y);
     }
   }
}

/*
Update the value of each pixel (x,y) of list pixels in the distance 
image dist_img by taking 1 + the minimum of its neighbors.
*/
void updateDistances(struct Image2D *dist_img, struct PixelList *pixels, 
     struct PixelList **pix_dict)
{
  /*printf("updateDistances\n");*/
  int x,y;
  int old_dist, new_dist, temp;
  while (PixelList_full(pixels)) 
  {
    PixelList_takefirst(pixels, &x, &y);
    new_dist = old_dist = Image2D_get(dist_img, x,y);
    if (old_dist==0) continue;
    
    for (int dx=x-1; dx<=x+1; dx++)
    for (int dy=y-1; dy<=y+1; dy++)
    {
       temp = 1+Image2D_get(dist_img, dx,dy);
       if (temp<new_dist) new_dist = temp;
    }
    if (new_dist<old_dist)
    {
       Image2D_put(dist_img, x,y, new_dist);
       /* propagate only to non-background pixels*/
       for (int dx=x-1; dx<=x+1; dx++)
       for (int dy=y-1; dy<=y+1; dy++)
       {
          if (Image2D_get(dist_img, dx,dy)>0) 
             PixelList_append(pixels, dx,dy); 
       }
       PixelList_append(&(*pix_dict)[new_dist], x,y);
    }
  } /* end while pixels not empty */
}

/*
The parameters are:
dist_img= the image storing distance,
max_dist= the maximum distance, 
pixel_dict= array of lists of pixels, where 
pixel_dict[d] contains the pixels with distance d.
Perform a cycle in decreasing distance order. At each step:
Find and remove from the image all the foreground squares of side max_dist.
Update in dist_img the distance of pixels affected by the removal,
Add them to pixel_dict[their updated smaller distance],
do not remove them from pixel_dict[their previous larger distance], 
they will be recongnized and discharged later.
*/
void defineSquares(struct Image2D *dist_img, int max_dist, 
     struct PixelList **pixel_dict,
     struct SquareList * squares)
{
  struct PixelList pixels;
  int x,y, x0,y0;
  int D;
  SquareList_init(squares);
  while(max_dist>0)
  {
    /*printf("Consider max dist=%d\n", max_dist);
    printf("  list=%p\n", &(*pixel_dict)[max_dist]);
    printPixels( &(*pixel_dict)[max_dist] );*/
    while (PixelList_full( &(*pixel_dict)[max_dist] ))
    {
      PixelList_takefirst( &(*pixel_dict)[max_dist], &x, &y);
      /*printf("Going to process %d %d\n",x,y);*/
      /* it may happen that (x,y) was given a smaller distance */
      if (Image2D_get(dist_img,x,y)==max_dist)
      {
        /*printf("Process it\n");*/
        /* define the min corner (x0,y0) and the side D of the square */
        x0 = y0 = -999;
        /* try to form an even square of side 2max_dist around a cluster
           of 2x2 foreground pixels, where one of them is (x,y):
           if success set (x0,y0) and D */
        if (Image2D_get(dist_img, x-1,y)==max_dist)
        {
          if (Image2D_get(dist_img, x,y-1)==max_dist && Image2D_get(dist_img, x-1,y-1)==max_dist)
          {   D = 2*max_dist; x0 = x-max_dist; y0 = y-max_dist; }
          else if (Image2D_get(dist_img, x,y+1)==max_dist && Image2D_get(dist_img, x-1,y+1)==max_dist)
          {   D = 2*max_dist; x0 = x-max_dist; y0 = y-max_dist+1; }
        }
        else if (Image2D_get(dist_img, x+1,y)==max_dist)
        {
          if (Image2D_get(dist_img, x,y-1)==max_dist && Image2D_get(dist_img, x+1,y-1)==max_dist)
          {  D = 2*max_dist; x0 = x-max_dist+1; y0 = y-max_dist; }
          else if (Image2D_get(dist_img, x,y+1)==max_dist && Image2D_get(dist_img, x+1,y+1)==max_dist)
          {  D = 2*max_dist; x0 = x-max_dist+1; y0 = y-max_dist+1; }
        }
        /* if not success, form an odd square centered in (x,y) with side 2max_dist-1 */
        if (x0==-999)
        {  D = 2*max_dist -1; x0 = x-max_dist+1; y0 = y-max_dist+1; }
        /* create the square */
        SquareList_append(squares , x0,y0,D);
        /* delete the square from the image */
        for (int dx=x0; dx<x0+D; dx++) 
        for (int dy=y0; dy<y0+D; dy++)
        {
           /*if (Image2D_get(dist_img, dx,dy)==0)
             printf("PROBLEM!!!\n");*/
           Image2D_put(dist_img, dx,dy, 0);
        }
        /* update distances */
        PixelList_init(&pixels);
        for (int xx=x0-1; xx<x0+D+1; xx++)
        {
          if (Image2D_get(dist_img, xx,y0-1)>0) PixelList_append(&pixels, xx,y0-1);
          if (Image2D_get(dist_img, xx,y0+D)>0) PixelList_append(&pixels, xx,y0+D);
        }
        for (int yy=y0; yy<y0+D; yy++)
        {
          if (Image2D_get(dist_img, x0-1,yy)>0) PixelList_append(&pixels, x0-1,yy);
          if (Image2D_get(dist_img, x0+D,yy)>0) PixelList_append(&pixels, x0+D,yy);
        }
        updateDistances(dist_img, &pixels, pixel_dict);
      } /* end if (x,y) has max_dist */
    } /* end while pixel_dict is full */
    max_dist--;
  }
}

/*
Main function: given a binary image img, find the
squares of its decomposition based on distance transform.
*/
void makeSquares(struct Image2D *img, struct SquareList *squares)
{
  struct Image2D img_dist;
  int max_dist;
  struct PixelList * pixel_dict;
  distanceTransformTwoScans(img, &img_dist, &max_dist, &pixel_dict);
  defineSquares(&img_dist, max_dist, &pixel_dict, squares);
}

/*--------------------------------------------------------------------*/

