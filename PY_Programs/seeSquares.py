"""
Read a text file containing a partition into squares
and show the squares in a graphic window.
"""

import matplotlib.pyplot as plt
import sys
from square import readSquares

def partition(squares, title=None, margin=False):
  """
  Show the squares of the partition, each square in a
  color, with black borders.
  """
  fig, ax = plt.subplots()
  for S in squares:
    x0,y0 = S.x0-0.5, S.y0-0.5
    x1,y1 = S.x0+S.side-0.5, S.y0+S.side-0.5
    y0 = -y0
    y1 = -y1
    ax.fill([x0,x1,x1,x0,x0],[y0,y0,y1,y1,y0])
    ax.plot([x0,x1,x1,x0,x0],[y0,y0,y1,y1,y0],"k",linewidth=0.5)
  if title:
    ax.set_title(title+" "+str(len(squares))+" squares", fontsize=20)
  else:
    ax.set_title(str(len(squares))+" squares", fontsize=20)
  maximum = max([S.x0+S.side for S in squares])
  maximum = max([S.y0+S.side for S in squares]+[maximum])
  #print(margin, maximum)
  if margin:
    delta=maximum*0.02
    ax.set(xlim=(-0.5-delta, maximum-0.5+delta), ylim=(-maximum+0.50-delta,0.5+delta))
    #ax.grid(True)
  else:
    ax.set(xlim=(-0.5, maximum-0.5), ylim=(-maximum+0.5,0.5))
  ax.set_xticks([],[]) #do not show numbers
  ax.set_yticks([],[]) #do not show numbers
  ax.set_box_aspect(1)
  #print("Decomposition has",len(squares),"squares")
  return fig, ax
  
def histogram(squares, title=None):
  """
  Show histogram of the square sizes.
  """
  fig, ax = plt.subplots()
  D = dict()
  for S in squares:
    L = S.side
    if not L in D: D[L] = 1
    else: D[L] += 1
  X = sorted(D.keys())
  Y = [D[L] for L in X]
  ax.bar(X,Y)
  if title:
    ax.set_title(title+" "+str(len(squares))+" squares", fontsize=20)
  else:
    ax.set_title(str(len(squares))+" squares", fontsize=20)
  return fig, ax

if __name__=="__main__":
  assert len(sys.argv)>1
  SQ = readSquares(sys.argv[1])
  f1, a1 = partition(SQ, margin=True)
  #f2, a2 = histogram(SQ)
  plt.show()
  if len(sys.argv)>2:
    exten, leng = "", len(sys.argv[2])
    if leng>4: exten =sys.argv[2][leng-4:]
    if exten==".png" or exten==".jpg":
       f1.savefig(sys.argv[2], bbox_inches="tight")
       print("Image 1 saved to "+sys.argv[2])
  #if len(sys.argv)>3:
  #  exten, leng = "", len(sys.argv[3])
  #  if leng>4: exten =sys.argv[3][leng-4:]
  #  if exten==".png" or exten==".jpg":
  #     f2.savefig(sys.argv[3])
  #     print("Image 2 saved to "+sys.argv[3])
