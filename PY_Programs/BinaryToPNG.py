"""
Read a binary file encoding a black and white image (for example one
of the files output by PNGtoBinary.py) and save the image
into PNG format.
This program is mainly used as test.
"""

import numpy
from PIL import Image

def readArrayFromBinary(filename):
  """
  Load the image from the binary file named filename.
  The file contains a header followed by the data.
  The header is 4 byte encoding the image dimensions 
  (one unsigned 2 byte each (little-endian byte order).
  Voxel data is 1 byte per voxel.
  """
  f = open(filename,"rb")
  content = f.read()
  f.close()
  #decode dimensions (on 2 bytes each)
  nx = int.from_bytes(content[0:2], byteorder='little', signed=False)
  ny = int.from_bytes(content[2:4], byteorder='little', signed=False)
  print("Dimensions",nx,ny)
  output = numpy.zeros((ny,nx), dtype=numpy.uint8)
  #decode the data
  black_count, white_count = 0, 0
  i = 4
  for x in range(nx):
    for y in range(ny):
        val = int.from_bytes(content[i:i+1], byteorder='little', signed=False)
        if val>0:  output[y][x] = 255; white_count += 1
        else: black_count += 1
        i += 1
  print("There are",white_count,"white and",black_count,"black pixels.")
  return output

def saveArrayToImage(iarray, filename):
  #Write array as image
  #Per le immagini della testa devo levare il mode, perche'?
  #hanno nbytes 1048576 e dtype int32
  #Per le immagini altre invece ci vuole
  #che hanno nbytes 65536 e dtype uint8
  #print(dir(image_array))
  #print(image_array.T) sono i valori dei pixel
  print(iarray.nbytes)
  print(iarray.dtype)
  if iarray.dtype==numpy.uint8:
     image = Image.fromarray(iarray, mode="L")
  else:
     image = Image.fromarray(iarray)
  image.save(filename)

def saveImage(img, file_name):
  #convert image to numpy array
  #we must rotate and invert the colors
  RUOTA = True
  INVERTI = False
  if RUOTA:
    output = numpy.zeros((img.dimY,img.dimX), dtype=numpy.uint8)
  else:
    output = numpy.zeros((img.dimX,img.dimY), dtype=numpy.uint8)
  if INVERTI:
    fore, back = 0, 255
  else:
    fore, back = 255, 0
  for x in range(img.dimX):
    for y in range(img.dimY):
      #note the exchanged x,y (rotate the image)
      if img.get(x,y)==0:  output[y][x] = back
      else: output[y][x] = fore
  #converto to pillow image
  image = Image.fromarray(output, mode="L")
  image.save(file_name)


if __name__=="__main__":
  import sys
  if len(sys.argv)<3:
    print("Need input (binary) file name and output (png) file name")
    sys.exit(1)
  I = readArrayFromBinary(sys.argv[1])
  saveArrayToImage(I, sys.argv[2])
