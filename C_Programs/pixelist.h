#ifndef PIXELS_HEADER_INCLUDED
#define PIXELS_HEADER_INCLUDED

/*
Information defining a pixel in a linked list.
The coordinates of the pixel are (x,y).
The next pointer connects this pixel to the following one
in the list.
*/
struct PixelNode {
  int x,y;
  struct PixelNode * next;
};

/*
Linked list of pixels, storing a pointer to the
first and to the last pixel.
*/
struct PixelList {
  struct PixelNode * first, * last;
};

/*
Initialize L as the empty list.
*/
extern void PixelList_init(struct PixelList *L);

/* 
Append the pixel (x,y) to the end of list L.
This is done with constant time complexity.
*/
extern void PixelList_append(struct PixelList *L, int x, int y);

/*
Print all pixels in list L to the standard output.
*/
extern void printPixels(struct PixelList *L);

/*
Take the coordinates x,y of the first pixel of the list L, 
delete that pixel from L. The list must not be empty.
*/
extern void PixelList_takefirst(struct PixelList *L, int *x, int *y);

/*
Return true if and only if the list L is not empty.
*/
extern int PixelList_full(struct PixelList *L);

/*
Delete the pixel of coordinates (x,y) from the list L.
Scan L from the beginning to find it.
The time complexity may be linear in the length of L.
*/
extern void PixelList_remove(struct PixelList *L, int x, int y);

/*
Return true if and only if the list contains pixel (x,y).
Scan L from the beginning to find it.
The time complexity may be linear in the length of L.
*/
extern int PixelList_contains(struct PixelList *L, int x, int y);

#endif
