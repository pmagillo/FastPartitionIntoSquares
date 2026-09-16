#Common source files and headers to all programs
SRC = main.c squares.c image2D.c
HDR = squares.h image2D.h

#Options to take execution time
OPTIONS = -D EXEC_TIME

#Main target
all: applyDT applyMDT applyQTR applySCAN applyFAST1 applyFAST2 applyFAST3

#Distance transform
applyDT: $(SRC) $(HDR) distance.c pixelist.c pixelist.h
	gcc $(OPTIONS) $(SRC) distance.c pixelist.c -o applyCDT

#Modified distance transform
applyMDT: $(SRC) $(HDR) mod_dist.c pixelist.c pixelist.h
	gcc $(OPTIONS) $(SRC) mod_dist.c pixelist.c -o applyMDT

#Quadtree
applyQTR: $(SRC) $(HDR) quadtree.c
	gcc $(OPTIONS) $(SRC) quadtree.c -o applyQTR

#Greedy scan
applySCAN: $(SRC) $(HDR) greedy3.c pixelist.c pixelist.h
	gcc $(OPTIONS) $(SRC) greedy3.c pixelist.c -o applySCAN

#New algorithm 1 (scan in increasing y direction)
applyFAST1: $(SRC) $(HDR) cut_squares.c
	gcc $(OPTIONS) $(SRC) cut_squares.c -o applyFAST1

#New algorithm 2 (repeat algorithm 1 four times after rotating the
#image 0, 90, 180, 270 degrees) and take the smallest partition)
applyFAST2: $(SRC) $(HDR) cut_four.c
	gcc $(OPTIONS) $(SRC) cut_four.c -o applyFAST2

#New algorithm 3 (change the scan direction if the current scan
#is in a situation where too many small squares are created)
applyFAST3: $(SRC) $(HDR) cut_spiral.c
	gcc $(OPTIONS) $(SRC) cut_spiral.c -o applyFAST3
