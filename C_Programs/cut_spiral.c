/*
Decompose a binary image into a collection of disjoint squares
with any side length, by using a single scan of the image.
Version scanning the image in four direction (concentric way).
*/

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "image2D.h"
#include "squares.h"

int biggestSquareXnegYpos(struct Image2D *img, int x0, int y0, int max_side)
{
/*  Given a foreground pixel of coordinates (x0,y0),
  find the side length of the biggest foreground square with (x0,y0) as its
  pixel with largest x, smallest y. 
  The square lies in negative x and positive y directions from (x0,y0).
  */
  int side = 1;
  int dist = 2;
  /*printf("INIZIO biggestSquareFromRight %d %d %d\n",x0,y0, max_side);*/
  int chance = 1; /* true */
  while (chance && dist<=max_side)
  {
    /* vertical left side of square with edge=dist */
    for (int d=0; d<dist; d++)                 /*  neg pos ok */
    {
      if (Image2D_get(img, x0-dist+1,y0+d)==0) /* ABC neg pos ok */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    /* horizontal bottom side of square with edge=dist */
    for (int d=1; d<dist; d++)    /* neg pos ok */
    {
      if (Image2D_get(img, x0-d+1,y0+dist-1)==0)  /* ABC neg pos ok */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    dist ++;
  }
  /*printf("biggestSquareXnegYpos (%d,%d) --> %d\n",x0,y0,(dist-1));*/
  return dist-1;
}

int biggestSquareXposYpos(struct Image2D *img, int x0, int y0, int max_side)
{
  /*
  Given a foreground pixel of coordinates (x0,y0),
  find the side length of the biggest foreground square with (x0,y0) as its
  pixel with smallest x, smallest y.
  The square lies in positive x and positive y directions from (x0,y0).
  */
  int side = 1;
  int dist = 2;
  /*printf("INIZIO biggestSquareFromLeft %d %d %d\n",x0,y0, max_side);*/
  int chance = 1; /* true */
  while (chance && dist<=max_side)
  {
    /* vertical left side of square with edge=dist */
    for (int d=0; d<dist; d++)  /*  pos pos ok */
    {
      if (Image2D_get(img, x0+dist-1,y0+d)==0)  /* ABC pos pos ok */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    /* horizontal bottom side of square with edge=dist */
    for (int d=1; d<dist; d++)    /*  pos pos ok */
    {
      if (Image2D_get(img, x0+d-1,y0+dist-1)==0)  /* ABC pos pos ok */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    dist ++;
  }
  /*printf("biggestSquareXposYpos (%d,%d) --> %d\n",x0,y0,(dist-1));*/
  return dist-1;
}

int biggestSquareXposYneg(struct Image2D *img, int x0, int y0, int max_side)
{
/*  Given a foreground pixel of coordinates (x0,y0),
  find the side length of the biggest foreground square with (x0,y0) as its
  pixel with smallest x, largest y.
  The square lies in positive x and negative y directions from (x0,y0).
  */
  int side = 1;
  int dist = 2;
  /*printf("INIZIO biggestSquareXposYneg (%d,%d) maxside=%d\n",x0,y0, max_side);*/
  int chance = 1; /* true */
  while (chance && dist<=max_side)
  {
    /* vertical left side of square with edge=dist */
    for (int d=0; d<dist; d++)   /*  pos neg err */
    {
      /* printf("1)Per d=%d considero pixel (%d,%d)\n",d,x0+dist-1,y0-d);*/
      if (Image2D_get(img, x0+dist-1,y0-d)==0)  /* ABC pos neg err */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    /* horizontal bottom side of square with edge=dist */
    for (int d=1; d<dist; d++)   /*  pos neg err */
    {
      /*printf("2)Per d=%d considero pixel (%d,%d)\n",d,x0+d-1,y0-dist+1);*/
      if (Image2D_get(img, x0+d-1,y0-dist+1)==0)   /* ABC pos neg err */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    dist ++;
  }
  /*printf("biggestSquareXposYneg (%d,%d) --> %d\n",x0,y0,(dist-1));*/
  return dist-1;
}

int biggestSquareXnegYneg(struct Image2D *img, int x0, int y0, int max_side)
{
/*  Given a foreground pixel of coordinates (x0,y0),
  find the side length of the biggest foreground square with (x0,y0) as its
  pixel with largest x, largest y.
  The square lies in negative x and negative y directions from (x0,y0). 
   */
  int side = 1;
  int dist = 2;
  /*printf("INIZIO biggestSquareFromRight %d %d %d\n",x0,y0, max_side);*/
  int chance = 1; /* true */
  while (chance && dist<=max_side)
  {
    /* vertical left side of square with edge=dist */
    for (int d=0; d<dist; d++)   /*  neg neg err */
    {
      if (Image2D_get(img, x0-dist+1,y0-d)==0) /* ABC neg neg err */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    /* horizontal bottom side of square with edge=dist */
    for (int d=1; d<dist; d++) /*  neg neg err */
    {
      if (Image2D_get(img, x0-d+1,y0-dist+1)==0) /* ABC neg neg err */
      {  chance = 0;  break; }
    }
    if (!chance) break;
    dist ++;
  }
  /*printf("biggestSquareXnegYneg (%d,%d) --> %d\n",x0,y0,(dist-1));*/
  return dist-1;
}


void makeSquaresAtYmin(struct Image2D *img, int y, struct SquareList *squares)
{
  /*
  Create all the foreground squares with lowest coordinate =y and 
  delete them from  the image.
  The image pixels on all rows <y are all background.
  */
  /*printf("*********makeSquaresAtYmin %d*****\n",y);*/
  /* Scan row y and find all the start and end pixels of a foreground run */
  SquareList_init(squares);
  int left_pix = -999, right_pix = -999;
  int run_length, side_from_left, side_from_right;
  int x0, side;
  for (int x=0; x<img->dimX; x++)
  {
  /*if (Image2D_get(img, x,y)==0) printf("  pixel bianco\n");*/
      if (Image2D_get(img, x,y)==0) continue;
      /* check if x is left corner */
      if (x==0 || Image2D_get(img, x-1,y)==0)
          left_pix = x;
      /* check if s is right corner */
      if (x==img->dimX-1 || Image2D_get(img, x+1,y)==0)
          right_pix = x;
      while (right_pix!=-999)
      {
#ifdef PARLA
/*     printf("Ymin  Trovato run da %d a %d\n", left_pix,right_pix);*/
#endif
          /* A run ends at x, process the run.
          There is a cycle because the run may get shortened several times
          #printf("Run from",left_pix,"to",right_pix) */
          run_length = right_pix-left_pix+1;
          if (run_length==1)
          {
#ifdef PARLA
            printf("Ymin  Quadrato con angolo sx %d e lato unitario\n", left_pix);
#endif
            SquareList_append(squares, left_pix,y, 1);
            Image2D_put(img, left_pix,y, 0);
            left_pix = right_pix = -999;
          }
          else
          {
            /* find square to be created */
            side_from_left = biggestSquareXposYpos(img, left_pix,y, run_length);
            side_from_right = biggestSquareXnegYpos(img, right_pix,y, run_length);
            if (side_from_left>=side_from_right)
            {   x0 = left_pix; side = side_from_left;  }
            else
            {   x0 = right_pix-side_from_right+1; side = side_from_right;  }
            /* save square */
#ifdef PARLA
            printf("Ymin  Quadrato con angolo %d,%d e lato %d\n", x0,y,side);
#endif
            SquareList_append(squares, x0,y, side);
            /* delete square from image */
            for (int dx=x0; dx<x0+side; dx++)
            for (int dy=y; dy<y+side; dy++)
            {
            /*printf("   Azzero pixel %d %d\n",dx,dy);*/
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
/*            if (left_pix>=0) printf("  run ridotto a %d %d\n",left_pix,right_pix);*/
        } /* end while: The run has been completely processed */
      }
  } /* end for: The row has been completely processed */
}

void makeSquaresAtYmax(struct Image2D *img, int y, struct SquareList *squares)
{
  /*
  Create all the foreground squares with highest coordinate =y and 
  delete them from  the image.
  The image pixels on all rows >y are all background.
  */
  /*printf("*********makeSquaresAtRow %d*****\n",y);*/
  /* Scan row y and find all the start and end pixels of a foreground run */
  SquareList_init(squares);
  int left_pix = -999, right_pix = -999;
  int run_length, side_from_left, side_from_right;
  int x0, side, y0;
  for (int x=0; x<img->dimX; x++)
  {
  /*if (Image2D_get(img, x,y)==0) printf("  pixel bianco\n");*/
      if (Image2D_get(img, x,y)==0) continue;
      /* check if x is left corner */
      if (x==0 || Image2D_get(img, x-1,y)==0)
          left_pix = x;
      /* check if s is right corner */
      if (x==img->dimX-1 || Image2D_get(img, x+1,y)==0)
          right_pix = x;
      while (right_pix!=-999)
      {
#ifdef PARLA
/*        printf("Ymax  Trovato run da %d a %d\n", left_pix,right_pix);*/
#endif
          /* A run ends at x, process the run.
          There is a cycle because the run may get shortened several times

          #printf("Run from",left_pix,"to",right_pix) */
          run_length = right_pix-left_pix+1;
          if (run_length==1)
          {
#ifdef PARLA
            printf("Ymax  Quadrato con angolo sx %d e lato unitario\n", left_pix);
#endif
            SquareList_append(squares, left_pix,y, 1);
            Image2D_put(img, left_pix,y, 0);
            left_pix = right_pix = -999;
          }
          else
          {
            /* find square to be created */
            side_from_left = biggestSquareXposYneg(img, left_pix,y, run_length);
            side_from_right = biggestSquareXnegYneg(img, right_pix,y, run_length);
            if (side_from_left>side_from_right) /* era >= */
            {   x0 = left_pix; side = side_from_left;  }
            else
            {   x0 = right_pix-side_from_right+1; side = side_from_right;  }
            y0 = y-side+1;
            /* save square */
#ifdef PARLA
      printf("Ymax  Quadrato con angolo sx %d e lato %d\n", x0,side);
#endif
            SquareList_append(squares, x0,y0, side);
            /* delete square from image */
            for (int dx=x0; dx<x0+side; dx++)
            for (int dy=y0; dy<y0+side; dy++)
            {
            /*printf("   Azzero pixel %d %d\n",dx,dy);*/
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
/*            if (left_pix>=0) printf("  run ridotto a %d %d\n",left_pix,right_pix);*/
        } /* end while: The run has been completely processed */
      }
  } /* end for: The row has been completely processed */
}


void makeSquaresAtXmin(struct Image2D *img, int x, struct SquareList *squares)
{
  /*
  Create all the foreground squares with left coordinate =x and 
  delete them from  the image.
  The image pixels on all columns <x are all background.
  */
  /*printf("*********makeSquaresAtColumn %d*****\n",x);*/
  /* Scan column x and find all the start and end pixels of a foreground run */
  SquareList_init(squares);
  int left_pix = -999, right_pix = -999;
  int run_length, side_from_left, side_from_right;
  int y0, side;
  for (int y=0; y<img->dimY; y++)
  {
  /*if (Image2D_get(img, x,y)==0) printf("  pixel bianco\n");*/
      if (Image2D_get(img, x,y)==0) continue;
      /* check if x is left corner */
      if (y==0 || Image2D_get(img, x,y-1)==0)
          left_pix = y;
      /* check if x is right corner */
      if (y==img->dimY-1 || Image2D_get(img, x,y+1)==0)
          right_pix = y;
      while (right_pix!=-999)
      {
#ifdef PARLA
/*          printf("Xmin  Trovato run da %d a %d\n", left_pix,right_pix);*/
#endif
          /* A run ends at y, process the run.
          There is a cycle because the run may get shortened several times
          #print("Run from",left_pix,"to",right_pix) */
          run_length = right_pix-left_pix+1;
          if (run_length==1)
          {
#ifdef PARLA
            printf("Xmin  Quadrato con angolo di min %d %d e lato unitario\n", x, left_pix);
#endif
            SquareList_append(squares, x,left_pix, 1);
            Image2D_put(img, x,left_pix, 0);
            left_pix = right_pix = -999;
          }
          else
          {
            /* find square to be created */
            /*printf("Calling  XnegYpos da pixel %d,%d\n",x,left_pix);*/
            side_from_left = biggestSquareXposYpos(img, x,left_pix, run_length);
            /*printf("Calling  XnegYneg da pixel %d,%d\n",x,right_pix);*/
            side_from_right = biggestSquareXposYneg(img, x,right_pix, run_length);
            if (side_from_left>side_from_right) /* it was >= */
            {   y0 = left_pix; side = side_from_left;  }
            else
            {   y0 = right_pix-side_from_right+1; side = side_from_right;  }
            /* save square */
#ifdef PARLA
            printf("Xmin  Quadrato con angolo %d %d e lato %d\n", x,y0,side);
#endif
            SquareList_append(squares, x,y0, side);
            /* delete square from image */
            for (int dx=x; dx<x+side; dx++)
            for (int dy=y0; dy<y0+side; dy++)
            {
              /*printf("   Azzero pixel %d %d\n",dx,dy);*/
                Image2D_put(img, dx,dy, 0);
            }
            /* update run */
            if (side==run_length) /* run disappears */
              left_pix = right_pix = -999;
            else /*  run becomes shorter */
            {
              if (y0==left_pix) left_pix += side;
              else             right_pix -= side;
            }
/*            if (left_pix>=0) printf("  run ridotto a %d %d\n",left_pix,right_pix);*/
        } /* end while: The run has been completely processed */
      }
  } /* end for: The row has been completely processed */
}


void makeSquaresAtXmax(struct Image2D *img, int x, struct SquareList *squares)
{
  /*
  Create all the foreground squares with right coordinate =x and 
  delete them from  the image.
  The image pixels on all columns >x are all background.
  */
  /*printf("*********makeSquaresAtColumn %d*****\n",x);*/
  /* Scan column x and find all the start and end pixels of a foreground run */
  SquareList_init(squares);
  int left_pix = -999, right_pix = -999;
  int run_length, side_from_left, side_from_right;
  int y0, side, x0;
  for (int y=0; y<img->dimY; y++)
  {
  /*if (Image2D_get(img, x,y)==0) printf("  pixel bianco\n");*/
      if (Image2D_get(img, x,y)==0) continue;
      /* check if x is left corner */
      if (y==0 || Image2D_get(img, x,y-1)==0)
          left_pix = y;
      /* check if x is right corner */
      if (y==img->dimY-1 || Image2D_get(img, x,y+1)==0)
          right_pix = y;
      while (right_pix!=-999)
      {
#ifdef PARLA
/*      printf("Xmax  Trovato run da %d a %d\n", left_pix,right_pix);*/
#endif
          /* A run ends at y, process the run.
          There is a cycle because the run may get shortened several times
          #print("Run from",left_pix,"to",right_pix) */
          run_length = right_pix-left_pix+1;
          if (run_length==1)
          {
#ifdef PARLA
            printf("Xmax  Quadrato con angolo di min y %d e lato unitario\n", left_pix);
#endif
            SquareList_append(squares, x,left_pix, 1);
            Image2D_put(img, x,left_pix, 0);
            left_pix = right_pix = -999;
          }
          else
          {
            /* find square to be created */
            side_from_left = biggestSquareXnegYpos(img, x,left_pix, run_length);
            side_from_right = biggestSquareXnegYneg(img, x,right_pix, run_length);
            if (side_from_left>=side_from_right)
            {   y0 = left_pix; side = side_from_left;  }
            else
            {   y0 = right_pix-side_from_right+1; side = side_from_right;  }
            x0 = x-side+1;
            /* save square */
#ifdef PARLA
            printf("Xmax  Quadrato con angolo %d %d e lato %d\n", x,y0,side);
#endif
            SquareList_append(squares, x0,y0, side);
            /* delete square from image */
            for (int dx=x0; dx<x0+side; dx++)
            for (int dy=y0; dy<y0+side; dy++)
            {
               /* printf("   Azzero pixel %d %d\n",dx,dy);*/
                Image2D_put(img, dx,dy, 0);
            }
            /* update run */
            if (side==run_length) /* run disappears */
              left_pix = right_pix = -999;
            else /*  run becomes shorter */
            {
              if (y0==left_pix) left_pix += side;
              else             right_pix -= side;
            }
/*            if (left_pix>=0) printf("  run ridotto a %d %d\n",left_pix,right_pix);*/
        } /* end while: The run has been completely processed */
      }
  } /* end for: The row has been completely processed */
}


#define TOLERANCE_ITER 6 /*6 3 max iterations with min acceptable edge length */
/* 10 iterazioni sono troppe */
#define TOLERANCE_EDGE 4 /*3 2 5 minimum acceptable edge length */

void makeSquares(struct Image2D *img, struct SquareList *squares)
{
  /*
  Decompose the foreground object contained in the given image into squares. 
  As side effect, the image at the end will be all background.
  Return the list of squares in which the object has been decomposed.
  */
  SquareList_init(squares);
  /* start with the entire image */
  int startX = 0, endX = img->dimX-1;
  int startY = 0, endY = img->dimY-1;
  /* cycle */
  int precedenti=0, adesso, number =0;
  while ((startX<endX) && (startY<endY))
  {/*INIZIO CICLO ESTERNO*/
#ifdef PARLA
  adesso = SquareList_len(squares);
  if (adesso>precedenti) {  printf("Finora %d squares\n", adesso); precedenti=adesso; }
  printf("Ciclo con immagine di dim X =%d,%d e Y =%d,%d\n",startX,endX, startY, endY);
#endif
    int ok = 1, small_iterations = 0;
/*    printf("INIZIO YMIN, ok %d startY %d endY %d\n",ok,startY,endY);*/
    while (ok && (startY<endY))
    {/*INIZIO CICLO YMIN*/
      /* find squares with corners of min y on row startY */
      struct SquareList aux;
      makeSquaresAtYmin(img, startY++, &aux);
      number = SquareList_len(&aux);
#ifdef PARLA
  printf("Finora %d squares\n", SquareList_len(squares));
  if (number==0) /*printf("Ymin creato nulla\n")*/;
  else printf("  Ymin=%d creati %d quadrati\n", startY-1, number);
#endif
      if (number>1 && SquareList_max_side(&aux)<=TOLERANCE_EDGE)
      {
        if (++small_iterations>=TOLERANCE_ITER) ok = 0;
#ifdef PARLA
        printf("Ymin  pochi(%d) quadrati e lato max=%d, small_iter=%d(risp. toll=%d)\n",number,SquareList_max_side(&aux),small_iterations,TOLERANCE_ITER);
        if (ok==0) printf("Cambio direzione da Ymin a Xmin\n");
#endif
      }
      else if (number>0) small_iterations = 0;
      SquareList_extend(squares, &aux);
    }/*FINE CICLO YMIN*/
/*    printf("FINE YMIN, ok %d startY %d endY %d\n",ok,startY,endY);*/
    if (startY==endY) break;
    ok = 1;
    small_iterations = 0;
/*    printf("INIZIO XMIN, ok %d startX %d endX %d\n",ok,startX,endX);*/
    while (ok && (startX<endX))
    {/*INIZIO CICLO XMIN*/
      /* find squares with corners of min x on column startX */
/*     printf("  cambio direzione\n");*/
      struct SquareList aux;
      makeSquaresAtXmin(img, startX++, &aux);
      number = SquareList_len(&aux);
#ifdef PARLA
  adesso = SquareList_len(squares);
  if (adesso>precedenti) {  printf("Finora %d squares\n", adesso); precedenti=adesso; }
  if (number==0) /*printf("creato nulla\n")*/;
  else   printf("  Xmin=%d creati %d quadrati\n", startX-1, number);
#endif
      if (number>1 && SquareList_max_side(&aux)<=TOLERANCE_EDGE)
      {
        if (++small_iterations>=TOLERANCE_ITER) ok = 0;
#ifdef PARLA
        printf("Xmin  pochi(%d) quadrati e lato max=%d, small_iter=%d(risp. toll=%d)\n",number,SquareList_max_side(&aux),small_iterations,TOLERANCE_ITER);
        if (ok==0) printf("Cambio direzione da Xmin a Ymax\n");
#endif
      }
      else if (number>0) small_iterations = 0;
      SquareList_extend(squares, &aux);
    }/*FINE CICLO XMIN*/
/*    printf("FINE XMIN, ok %d startX %d endX %d\n",ok,startX,endX);*/
    if (startX==endX) break;
    ok = 1;
    small_iterations = 0;
/*    printf("INIZIO YMAX, ok %d startY %d endY %d\n",ok,startY,endY);*/
    while (ok && (startY<endY))
    {/*INIZIO CICLO YMAX*/
      struct SquareList aux;
      /* find squares with corners of max y on row endY */
      makeSquaresAtYmax(img, endY--, &aux);
      number = SquareList_len(&aux);
#ifdef PARLA
  adesso = SquareList_len(squares);
  if (adesso>precedenti) {  printf("Finora %d squares\n", adesso); precedenti=adesso; }
  if (number==0) /*printf("creato nulla\n")*/;
  else   printf("  Ymax=%d creati %d quadrati\n", endY+1, number);
#endif
      if (number>1 && SquareList_max_side(&aux)<=TOLERANCE_EDGE)
      {
        if (++small_iterations>=TOLERANCE_ITER) ok = 0;
#ifdef PARLA
        printf("Ymax  pochi(%d) quadrati e lato max=%d, small_iter=%d(risp. toll=%d)\n",number,SquareList_max_side(&aux),small_iterations,TOLERANCE_ITER);
        if (ok==0) printf("Cambio direzione da Ymax a Xmax\n");
#endif
      }
      else if (number>0) small_iterations = 0;
      SquareList_extend(squares, &aux);
    }/*FINE CICLO YMAX*/
/*    printf("FINE YMAX, ok %d startY %d endY %d\n",ok,startY,endY);*/
    if (startY==endY) break;
    ok = 1;
    small_iterations = 0;
/*    printf("INIZIO XMAX, ok %d startX %d endX %d\n",ok,startX,endX);*/
    while (ok && (startX<endX))
    {/*INIZIO CICLO XMAX*/
      /* find squares with corners of max x on column endX */
      struct SquareList aux;
      makeSquaresAtXmax(img, endX--, &aux);
      number = SquareList_len(&aux);
#ifdef PARLA
  adesso = SquareList_len(squares);
  if (adesso>precedenti) {  printf("Finora %d squares\n", adesso); precedenti=adesso; }
  if (number==0) /*printf("creato nulla\n")*/;
  else   printf("  Xmax=%d creati %d quadrati\n", endX-1, number);
#endif
      if (number>1 && SquareList_max_side(&aux)<=TOLERANCE_EDGE)
      {
        if (++small_iterations>=TOLERANCE_ITER) ok = 0;
#ifdef PARLA
        printf("Xmax  pochi(%d) quadrati e lato max=%d, small_iter=%d(risp. toll=%d)\n",number,SquareList_max_side(&aux),small_iterations,TOLERANCE_ITER);
        if (ok==0) printf("Cambio direzione da Xmax a Ymin\n");
#endif
      }
      else if (number>0) small_iterations = 0;
      SquareList_extend(squares, &aux);
    }/*FINE CICLO XMAX*/
/*    printf("FINE XMAX, ok %d startX %d endX %d\n",ok,startX,endX);*/
#ifdef PARLA
    printf("Qui  dim X =%d,%d e Y =%d,%d\n",startX,endX, startY, endY);/*****************/
#endif
  }/*FINE CICLO PRINCIPALE*/
#ifdef PARLA
  printf("Ciclo finito con immagine di dim X =%d,%d e Y =%d,%d\n",startX,endX, startY, endY);
  printf("Alla fine del ciclo principale ci sono %d squares\n", SquareList_len(squares));
#endif
  
  /* all remaining pixels on row startY==endY or on column startX==endX are singleton squares,
     only one of the two cycles may have more than one element */
  for (int x=startX; x<=endX; x++)
  for (int y=startY; y<=endY; y++)
  {
    if (Image2D_get(img, x,y)>0)
       SquareList_append(squares, x,y, 1);
  }
}

