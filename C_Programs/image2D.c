#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include "image2D.h"

/* See image2D.h for explaining comments. */
 
void Image2D_init(struct Image2D *self, int dimX, int dimY)
{
      self->dimX = dimX;
      self->dimY = dimY;
      self->cells = (int **) malloc(dimX*sizeof(int *));
      for (int x=0; x<dimX; x++)
      {
        self->cells[x] = (int*) malloc(dimY*sizeof(int));
        for (int y=0; y<dimY; y++) self->cells[x][y] = 0;
      }  
}

void Image2D_put(struct Image2D *self, int x, int y, int col)
{
  self->cells[x][y] = col;
}

int Image2D_get(struct Image2D *self, int x, int y)
{
  if (x<0 || x>=self->dimX || y<0 || y>=self->dimY)
      return 0;
  return self->cells[x][y];
}

int Image2D_number(struct Image2D *self, int color)
{
  int N = 0;
  for (int x=0; x<self->dimX; x++)
      for (int y=0; y<self->dimY; y++)
         if (self->cells[x][y]==color) N ++;
    return N;
}

void readBinary(struct Image2D *img, char *filename)
{
  FILE *f = fopen(filename,"rb");
  int c;
  int nx, ny, val;
  unsigned char aux[2];
  /*decode dimensions (on 2 bytes each)*/
  c = fread (aux, 1/*size*/, 2/*count*/, f);
  nx = aux[1]*256+aux[0];
  c = fread (aux, 1/*size*/, 2/*count*/, f);
  ny = aux[1]*256+aux[0];
  /*printf("Dimensions %d %d\n",nx,ny);*/
  /* decode the data */
  Image2D_init(img, nx, ny);
  for (int x=0; x<nx; x++)
  for (int y=0; y<ny; y++)
  {
     c = fread (&aux, 1,1, f);
     val = aux[0];
     Image2D_put(img, x,y, val);
  }
  fclose(f);
}

void writeBinary(struct Image2D *img, char *filename)
{
  FILE * f = fopen(filename,"wb");
  unsigned char aux[2];
  int c;
  /* encode dimensions (on 2 bytes each) */
  /*printf("Dimensions %d %d\n", img->dimX, img->dimY);*/
  aux[1] = img->dimX / 256;
  aux[0] = img->dimX % 256;
  c = fwrite(aux, 1/*size*/, 2/*count*/, f);
  aux[1] = img->dimY / 256;
  aux[0] = img->dimY % 256;
  c = fwrite(aux, 1/*size*/, 2/*count*/, f);
  /* encode the data */
  for (int x=0; x<img->dimX; x++)
  for (int y=0; y<img->dimY; y++)
  {
     aux[0] = Image2D_get(img, x,y);
     fwrite(aux, 1,1, f);
  }
  fclose(f);
}

void copyImage(struct Image2D *src, struct Image2D *dst, int rotation)
{
  if (rotation==0 || rotation==2)
      Image2D_init(dst, src->dimX, src->dimY);
  else if (rotation==1 || rotation==3)
      Image2D_init(dst, src->dimY, src->dimX);
  else
  {
    return; /* rotation is not valid */
  }
  for (int x=0; x<src->dimX; x++)
  for (int y=0; y<src->dimY; y++)
  {
    if (rotation==0)
       Image2D_put(dst, x, y, Image2D_get(src, x,y));
    else if (rotation==1)
       Image2D_put(dst, (src->dimY-y-1), x, Image2D_get(src, x,y));
    else if (rotation==2)
       Image2D_put(dst, (src->dimX-x-1), (src->dimY-y-1), Image2D_get(src, x,y));
    else /* ==3 */
       Image2D_put(dst, y, (src->dimX-x-1), Image2D_get(src, x,y));
  }
}
