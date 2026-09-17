#ifndef SQUARES_HEADER_INCLUDED
#define SQUARES_HEADER_INCLUDED

/*
Information defining a square in a linked list.
The square has side pixels on each side and (x0,y0) are the 
coordinates of the pixel with minimum x and minimum y.
The next pointer connects this square to the following one
in the list.
*/
struct SquareNode {
  int x0, y0; /* corner of minimum x and y */
  int side;   /* side length */
  struct SquareNode * next;
};

/*
Linked list of squares, storing a pointer to the
first and to the last square, and the element count.
*/ 
struct SquareList {
  struct SquareNode * first, * last;
  int length;
};

/*
Initialize L as the empty list.
*/
extern void SquareList_init(struct SquareList *L);

/* 
Append the square of minimum pixel (x,y) and side length s
to the end of list L.
This is done with constant time complexity.
*/
extern void SquareList_append(struct SquareList *L, int x, int y, int s);

/*
Append all elements of list T to the end of list L. 
As a side effect T becomes empty.
This operation is done in linear time in the length of list T.
*/
extern void SquareList_extend(struct SquareList *L, struct SquareList *T);

/*
Print all squares in list L to the file named file_name.
*/
extern void printSquares(struct SquareList *L, char *file_name);

/*
Return the length of the list L (with constant time complexity).
*/
extern int SquareList_len(struct SquareList *L);

/*
Take the minimum coordinates x,y and the side length s of the
first square of the list L, delete such square from L.
The list must not be empty.
*/
extern void SquareList_takefirst(struct SquareList *L, int *x, int *y, int *s);

/*
Return true if and only if the list L is not empty.
*/
extern int SquareList_full(struct SquareList *L);

/*
Put into dstL a shallow copy of the list srcL (i.e., the nodes are
exactly the same, not copies of them).
If dstL was not empty, its memory is first disposed.
*/
extern void SquareList_copy(struct SquareList *srcL, struct SquareList *dstL);

/*
L is a list of squares generated on an image that was rotated
with respect to the original image. Rotate back the squares 
contained into L, to become squares referring to the original
(not rotated) image. Rotation is 0 (no rotation), 1 (90 degrees),
2 (180 degreed) or 9 (270 degrees).
*/
extern void SquareList_rotate(struct SquareList *L,
     int dimX, int dimY, int rotation);

/*
Return the maximum side length of a square in the list L.
This is done by scanning the list, with linear time complexity
in its length.
*/
extern int SquareList_max_side(struct SquareList *L);

#endif
