#!/usr/bin/env python3

import tlay2_monitor
import palette

m=tlay2_monitor.tlay2_monitor()

m.inb(0x3DA)
m.outb(0x3C2,0xE3) #VGA_MISC_WRITE

seq = [0x03, 0x01, 0x08, 0x00, 0x06]
for i in range(len(seq)):
    m.outb(0x3C4,i) #VGA_SEQ_INDEX
    m.outb(0x3C5,seq[i]) #VGA_SEQ_DATA

m.outb(0x3D4, 0x03); #VGA_CRTC_INDEX
m.outb(0x3D5, 
        m.inb(0x3D5) | 0x80); #VGA_CRTC_DATA
m.outb(0x3D4, 0x11); #VGA_CRTC_INDEX
m.outb(0x3D5, 
        m.inb(0x3D5) & ~0x80); #VGA_CRTC_DATA

crtc = [
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,]
for i in range(len(crtc)):
    m.outb(0x3D4,i) #VGA_CRTC_INDEX
    m.outb(0x3D5,crtc[i]) #VGA_CRTC_DATA

gc=[
    	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,]
for i in range(len(gc)):
    m.outb(0x3CE,i) #VGA_GC_INDEX
    m.outb(0x3CF,gc[i]) #VGA_GC_DATA
ac = [
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
        ]
for i in range(len(ac)):
    m.outb(0x3C0,i) #VGA_AC_INDEX
    m.outb(0x3C0,ac[i]) #VGA_AC_DATA
m.inb(0x3DA) #VGA_INSTAT_READ
m.outb(0x3C0,0x20) #VGA_AC_INDEX

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


im_pal = dict()

i=0
for c in im.getdata():
    if not c in im_pal.keys():
        r=(pal_pal[c*3]//85)
        g=(pal_pal[c*3+1]//85)
        b=(pal_pal[c*3+2]//85)
        im_pal[c] = (0 |
            ((r&1)<<5) | ((r&2)<<1) |
            ((g&1)<<4) | ((g&2)<<0) |
            ((b&1)<<3) | ((b&2)>>1))
        i+=1

#ivd = {c: i for i,c in im_pal.items()}


for plane in range(4):
    set_plane(plane)
    s=BitArray()
    for c in im.getdata():
        s += '0b1' if c&(1<<plane) else '0b0'
    m.putmem(0xA0000,s.tobytes())

ega_pal = ((k,v) for k,v in im_pal.items())
#for k,v in im_pal.items():
#    ega_pal.append((k,v))

palette.set_ega_palette(m,ega_pal)
