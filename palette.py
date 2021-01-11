#!/usr/bin/env python3

import tlay2_monitor

m=tlay2_monitor.tlay2_monitor()

m.outb(0x3C6,0xff)

def get_ega_palette(self, indexes=range(16)):
    self.inb(0x3DA)
    ret = []
    for i in indexes:
        self.outb(0x3C0,i)
        ret.append((i,self.inb(0x3C1)))
        self.inb(0x3DA)
    self.outb(0x3C0,0x20) #VGA_AC_INDEX
    return ret

def set_ega_palette(self, data):
    self.inb(0x3DA)
    ret = []
    for i,val in data:
        self.outb(0x3C0,i)
        self.outb(0x3C0,val)
    self.outb(0x3C0,0x20) #VGA_AC_INDEX
    return ret



def get_palette2(self,size=256):

    self.outb(0x3C7,0)
    return tuple( # we want here results not a generator 
            (self.inb(0x3C9),
             self.inb(0x3C9),
             self.inb(0x3C9),
            )
            for i in range(0,size)
           )

def set_palette2(self,palette):
    self.outb(0x3C8,0)
    for col in palette:
        self.outb(0x3C9,col[0])
        self.outb(0x3C9,col[1])
        self.outb(0x3C9,col[2])

if __name__=="__main__":
    pal = get_ega_palette(m)
    print(pal)
    set_ega_palette(m,pal)
