"""
Convert a black and white image given in one of the usual formats
(e.g., PNG format) to a binary file.
The output can be used as input of one of the C programs
for decomposing an image into squares.
The conversion from image to binary may swap the black and white,
to obtain that the background of the binary file is black.
"""

from image2D import writeBinary
from reading2D import adaptiveRead
import sys

def adjustForeBack(img):
  # determine background color
  V = [img.get(x,y) for x,y in [(0,0),(img.dimX-1,0),(0,img.dimY-1),(img.dimX-1,img.dimY-1)]]
  #for x,y in [(0,0),(img.dimX-1,0),(0,img.dimY-1),(img.dimX-1,img.dimY-1)]:
  #  print("Corner",x,y,"color",img.get(x,y))
  if V.count(0)>2: background=0
  else: background=1
  #print("count zeroes",V.count(0),"background",background)
  if background == 0: return img
  print("Inverting image")
  # invert the image 
  for x in range(img.dimX):
    for y in range(img.dimY):
      if img.get(x,y)==0: img.put(x,y, 1)
      else: img.put(x,y, 0)
  #for x,y in [(0,0),(img.dimX-1,0),(0,img.dimY-1),(img.dimX-1,img.dimY-1)]:
  #  print("Now corner",x,y,"color",img.get(x,y))
  return img

def rotate(img,times90):
  from image2D import Image2D
  # rotate by 90 * times90 degrees
  #  90:  (x,y) ---> (dimY-y-1, x)
  # 180:  (x,y) ---> (dimX-x-1, dimY-y-1)
  # 270:  (x,y) ---> (y, x)
  if times90==1:
    T = lambda x,y : (img.dimY-y-1, x)
  elif times90==2:
    T = lambda x,y : (img.dimX-x-1, img.dimY-y-1)
  elif times90==3:
    T = lambda x,y : (y, img.dimX-x-1)
  else: assert False
  
  if times90%2==1: # 90 or 270 degrees
    output = Image2D(img.dimY,img.dimX)
  else:
    output = Image2D(img.dimX,img.dimY)
  for x in range(img.dimX):
    for y in range(img.dimY):
       xx,yy = T(x,y)
       output.put(xx,yy, img.get(x,y))
  return output

def main_rotating(argv):
  try:
    assert len(argv)>=4
    R = int(argv[2])
    assert R in (1,2,3)
    print("Reading image from", argv[1]) 
    IMG = adaptiveRead(argv[1])
    IMG = rotate(IMG, R)
    from BinaryToPNG import saveImage
    saveImage(IMG,argv[3])
    print("Writing rotated image to",argv[3])
  except:
    print("Need input_image, times_to_rotate, output_image");
    raise

def main(argv):
  try:
    assert len(argv)>=3
    print("Reading image from", argv[1]) 
    IMG = adaptiveRead(argv[1])
    IMG = adjustForeBack(IMG)
    print("Writing binary file to",argv[2])
    writeBinary(IMG, argv[2])
    #from BinaryToPNG import saveImage  
    #saveImage(IMG,"salvata.png")
  except:
    print("Need input_image and output_binary_name");
    raise

if __name__=="__main__":
  main(sys.argv)
  #main_rotating(sys.argv)

"""
Example:
python3 PNGtoBinary.py ~/Desktop/BUONO/DATA_2D/CzechLeaves/thresholded_trees/Vitis_riparia_5.png riparia.binary
"""
