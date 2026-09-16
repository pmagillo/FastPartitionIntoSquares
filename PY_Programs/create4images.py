"""
Read a black and white image;
recongnize which is the background color and, if necessary,
swap black and white in such a way to have black as backgound;
convert the image to binary format in four versions,
rotated 0, 90, 180 and 270 degrees.

The parameters on command line are:
the name of image to read, and (optionally)
the prefix for the binary files to write.
If the prefix is not given, it will be "tempimage"
and the output files will be:
tempimage_R0.binary, tempimage_R1.binary,
tempimage_R2.binary, tempimage_R3.binary 
"""

from image2D import writeBinary
from reading2D import adaptiveRead
from PNGtoBinary import rotate, adjustForeBack

def main(argv):
  try:
    if len(argv)>2: prefix = argv[2]
    else: prefix = "tempimage"
    print("Reading image from", argv[1]) 
    IMG = adaptiveRead(argv[1])
    IMG = adjustForeBack(IMG)
    for R in (0,1,2,3):
      if R>0: TEMP = rotate(IMG,R)
      else: TEMP = IMG
      name = prefix+"_R"+str(R)+".binary"
      print("Writing binary file to",name)
      writeBinary(TEMP, name)
  except:
    print("Need input_image [output_binary_prefix]");
    raise

if __name__=="__main__":
  import sys
  main(sys.argv)
