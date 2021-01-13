#!/usr/bin/env python3

import tlay2_monitor
import palette

m=tlay2_monitor.tlay2_monitor()

m.runfunc(0)

def set_plane(p):
    p &= 3
    pmask = 1 << p
#/* set read plane */
    m.outb(0x3CE, 4) #VGA_GC_INDEX
    m.outb(0x3Cf, p) #VGA_GC_DATA
#/* set write plane */
    m.outb(0x3C4, 2) #VGA_SEQ_INDEX
    m.outb(0x3C5, pmask) #VGA_SEQ_DATA

from PIL import Image
import sys
im=Image.open(sys.argv[1]).convert("RGB")
im=im.resize((640,480))
pal_image= Image.new("P", (1,1))
pal_pal = list()
for i in range(64):
    pal_pal.extend((
    (i>>2&1)*85+(i>>5&1)*170, 
    (i>>1&1)*85+(i>>4&1)*170,
    (i>>0&1)*85+(i>>3&1)*170))
pal_pal.extend((0,0,0)*(256-64))
pal_image.putpalette(
#print(
   pal_pal)
im=im.quantize(palette=pal_image)
im=im.quantize(colors=16)
im.save("tmp.png")
pal_pal = im.getpalette()
from bitstring import BitArray

p = list()
for c in im.getdata():
    if c not in p:
        p.append(c)
p.sort()
print(p)

im_pal = []

for i in range(16):
        r=pal_pal[i*3]
        g=pal_pal[i*3+1]
        b=pal_pal[i*3+2]
        im_pal.append((r,g,b))

class LineSelector:
    def __init__(self,line):
        self.line = line
    def getdata(self):
        method = Image.EXTENT
        data = (0, line, 640, line+1)
        return method, data

for line in range(480):
    b=im.transform((640,1),LineSelector(line)).getdata()
    import struct
    m.runfunc(1,struct.pack("<HH",0,line)+bytes(b))

print(im_pal)

ega_pal = b''
for pal in im_pal:
    ega_pal+=struct.pack("<BBB",pal[0],pal[1],pal[2])

m.runfunc(2,struct.pack("<B",0)+ega_pal)
