verbose = False

class Square:
  """
  The objects of this class are squares. A square stores
  the coordinates (x,y) of its pixel of smallest x, smallest y,
  and the length of its side in pixels.
  """
  def __init__(self, x,y,side=1):
    self.x0 = x
    self.y0 = y
    self.side = side
    if verbose: print("New square corner",x,y,"side",side)

  def __str__(self):
    return str(self.x0)+" "+str(self.y0)+" "+str(self.side)

def printSquares(squares, file_name=None):
  """
  Write a sequence of squares to standard output or
  to a file (if a file name is given).
  The format is text and each square is written as
  x0 y0 sd
  where (x0,y0) are the coordinates of the pixel of minimum x,y
  and sd is the side.
  """
  if file_name==None:
    # standard output
    for s in squares:
       print(s)
  else:
    # open file
    F = open(file_name,"w")
    for s in squares:
      F.write(str(s)+"\n")
    F.close()

def readSquares(file_name):
  """
  Read a list of squares to from a file.
  The format is text and each square is given as
  x0 y0 sd
  where (x0,y0) are the coordinates of the pixel of minimum x,y
  and sd is the side.
  """
  squares = []
  F = open(file_name,"r")
  for L in F:
    parts = L.split()
    x0, y0, side = [int(p) for p in parts]
    squares.append(Square(x0, y0, side))
  F.close()
  return squares
