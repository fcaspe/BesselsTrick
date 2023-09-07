from PIL import Image
import numpy as np

def set_background(img,r,g,b):
  
  mask = (image[:,:,3] == 0)
  image[mask,0] = r
  image[mask,1] = g
  image[mask,2] = b
  return image

def change_color(img,ir,ig,ib,dr,dg,db):
  
  mask = ((image[:,:,0] == ir) * (image[:,:,1] == ig) * (image[:,:,2] == ib))
  image[mask,0] = dr
  image[mask,1] = dg
  image[mask,2] = db
  return image


image = Image.open('algorithms.png')

print(image.size)
image = np.asarray(image).copy()
print(image.shape)

jump_y = (image.shape[0]-2)//8
jump_x = image.shape[1]//4-2

# Background color 12083D = 18,8,61
image = set_background(image,18,8,61)
image = change_color(image,255,255,255,255,0,0)
image = change_color(image,0,0,0,255,255,255)
image = change_color(image,255,0,0,127,127,127)
#image[image[:,:,3] == 0] = 255

chunk = image[0:jump_y,0:jump_x,0:3]

for y in range (8):
  for x in range(4):
    chunk = image[y*jump_y:jump_y*(y+1),
                  x*jump_x:jump_x*(x+1),0:3]

    im = Image.fromarray(chunk)
    im.save(f"algo_{1+x+y*4}.png")