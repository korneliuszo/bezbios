#!/usr/bin/env python3
import vesa_low

m=vesa_low.vesa()

m.set_video_mode(259|0x4000)

from PIL import Image
from PIL import ImageColor
import sys
im=Image.open(sys.argv[1]).convert("RGB")
im=im.resize((m.modeinfo['XResolution'],m.modeinfo['YResolution']))

pixels = b''

for pix in im.getdata():
    r = pix[0] >> 5
    g = pix[1] >> 5
    b = pix[2] >> 6
    c = r << 5 | g << 2 | b
    pixels+=bytes([c])

m.putpixels(pixels)

import struct
ega_pal = b''
for i in range(256):
    r=(i>>5) * 63 // 7
    g=((i>>2)&0x7) * 63 // 7
    b=((i)&0x3) * 63 // 3
    pal = (r,g,b)
    ega_pal+=struct.pack("<BBB",pal[0],pal[1],pal[2])

m.setpalette(ega_pal)