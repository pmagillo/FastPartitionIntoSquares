# FastPartitionIntoSquares
Algorithms to partition a 2D object (the foreground of a black and white image) into squares

This repository contains the software associated with the paper
"Fast partition of a 2D digital object into squares"
by Paola Magillo and Lidija Comic (2026)

The software is divided into two parts:

A) C Programs to partition a black and white image into squares:

  1) Distance transform
  2) Modified distance transform described in the paper
  3) Quadtree
  4) Greedy scan
  5) new Algorithm 1 proposed in the paper
  6) new Algorithm 2 proposed in the paper
  7) new Algorithm 3 proposed in the paper
 
  References for the algorithms can be found in the paper.
 
  All the above programs are compiled with the given makefile.
  See the makefile for the list of sources and headers.
 
  All executables take as input an image encoded in binary format
  and give as output a text file listing the squares.

  They do not use any library, not even library to manage images.

B) Accessory Python programs:

  PNGtoBinary.py
    Take a black and white image and generate a binary file.
    Used to generate the input format accepted by the C programs.

  BinaryToPNG.py
    Opposite transformation (just to check).

  create4images.py
    Take a black and white image and generate four binary files,
    representing the same image and three rotated versions by
    90, 180 and 270 degrees.

  The above python programs require numpy and pillow (PIL) packages.

  seeSquares.py
    Take a text file containin squares and visualize the squares.
    Used to see the results of the C programs.

  The above python program needs the matplotlib package.

  Other python sources, not listed here, define auxiliary functions.
