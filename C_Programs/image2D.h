#ifndef IMAGE_2D_HEADER_INCLUDED
#define IMAGE_2D_HEADER_INCLUDED

/*
Information defining a 2D image. If the image is binary,
the colors are 1 (foreground) and 0 (background).
*/
struct Image2D
{
  int dimX, dimY; /* number of pixels horizontally and vertically */
  int ** cells;   /* 2D array of colors */
};

/*
Create a 2D image with dimX, dimY pixels on the two sides,
where all pixels are zero (background).
*/ 
extern void Image2D_init(struct Image2D *self, int dimX, int dimY);
      
/*
Put the given color col into pixel of coordinates (x,y).
It must be x>=0 and x<self->dimX and y>=0 and y<self->dimY.
*/
extern void Image2D_put(struct Image2D *self, int x, int y, int col);

/*
Get and return the color of the pixel of coordinates (x,y).
Pixels outside image are considered as zero (background).
*/
extern int Image2D_get(struct Image2D *self, int x, int y);

/*
Return the number of pixels of the image with given color.
The implementation counts the pixels. 
The time complexity is linear in the total number of pixels
(i.e., width x height of the image).
*/
extern int Image2D_number(struct Image2D *self, int color);

/*
Load the image from the binary file named filename.
The file contains a header followed by the data.
The header is 4 byte encoding the image dimensions 
(one unsigned 2 byte each (little-endian byte order).
Voxel data is 1 byte per voxel.
*/
extern void readBinary(struct Image2D *img, char *filename);

/*
Write the image to the binary file named filename.
The file contains a header followed by the data.
The header is 4 byte encoding the image dimensions 
(one unsigned 2 byte each (little-endian byte order).
Voxel data is 1 byte per voxel.
*/
extern void writeBinary(struct Image2D *img, char *filename);

/*
Create a copy of the image src and put the result in the image dst.
The dst image is allocated within this function.
Rotation must be 0 (no rotation), 1 (90 degrees), 2 (180 degrees),
or 3 (270 degrees). If rotation is not 0, the dst image will be
rotated that number of degrees wrt the src image. Rotation direction
is clockwise.
*/
extern void copyImage(struct Image2D *src, struct Image2D *dst, int rotation);

#endif
