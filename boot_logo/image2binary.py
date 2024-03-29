#this python script is quite slow for larger images >_<
from PIL import Image                                         #Import Image from Pillow

paletteFile = "colors.png"                                    #palette the BIOS uses
convertFile = "hutao.png"                                       #image to turn into a binary
outputFile  = "image.img"                                     #name of output file

pal = Image.open(paletteFile).convert('RGB')
palette = pal.load()                                          #load pixels of the palette
image = Image.open(convertFile).convert('RGB')
pixels = image.load()                                         #load pixels of the image

binary = open(outputFile, "wb")                                #clear/create binary file

list = []                                                     #create a list for the palette
for y in range(pal.height):
    for x in range(pal.width):
        list.append(palette[x,y])                             #save the palette into an array

binary.write(bytearray([image.width&0xFF])) #write width and height as the first two bytes for variable image size
binary.write(bytearray([image.height&0xFF]))
data = []
print(image.width&0xFF,image.height&0xFF)
# print("{",end="")
for y in range(image.height):
    # x = image.width - x - 1                                   #invert x-axis (for shorter assembly code)
    for x in range(image.width):
        # y = image.height - y - 1                              #invert y-axis (for shorter assembly code)
        difference = 0xFFFFFFF                                #init difference with a high value
        choice = 0                                            #the index of the color nearest to the original pixel color
        index = 0                                             #current index within the palette array
        for c in list:
            dif = sum([(pixels[x,y][i] - c[i])**2 for i in range(3)]) #calculate difference for RGB values
            if dif < difference:
                difference = dif
                choice = index
            index += 1
        # print("x: "+ str(x) +" y: "+ str(y), end=" ")
        # print(choice&0xFF)
        binary.write(bytearray([choice&0xFF]))                #write nearest palette index into binary file as 1 byte
binary.close()                                                #close file handle
# print("-99};",end="\n")
print("Done.")