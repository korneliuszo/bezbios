#!/usr/bin/env python3

import tlay2_monitor
import tlay2_v86_monitor
import struct

class e820():
    SMAP_int= int.from_bytes(b'SMAP',"big")

    def __init__(self):
        self.v86monitor = tlay2_v86_monitor.tlay2_v86_monitor()
        self.monitor = tlay2_monitor.tlay2_monitor()  

    def get_data_chunk(self,continuation):
        regs = self.v86monitor.get_emptyregs()
        scratch = self.v86monitor.get_scratchaddr()
        regs["ax"] = 0xE820
        regs["bx"] = continuation
        regs["es"] = scratch["segment"]
        regs["di"] = scratch["offset"]
        regs["cx"] = 30
        regs["dx"] = self.SMAP_int
        self.v86monitor.vm86_int_call(0x15,regs)
        if(regs["ax"]!=self.SMAP_int):
            return None
        
        buff = self.monitor.getmem(scratch["linear"],regs["cx"])
        
        return regs["bx"],buff

    def get_all_chunks(self):
        i=0
        ret = []
        while True:
            a=self.get_data_chunk(i)
            i=a[0]
            ret.append(a[1])
            if i==0:
                break
        return ret
    
    class chunk():
        def __init__(self,base,length,type):
            self.base = base
            self.length = length
            self.type = type
        def __str__(self):
            return f'Base: 0x{self.base:08x} Len: 0x{self.length:08x} Type: {self.type}'
    
    def decode_chunk(self,chunk):
        base, length,type = struct.unpack("<QQI",chunk[:20])
        return self.chunk(base, length,type)
    
        
    
if __name__ == "__main__":
    p=e820()
    for ch in p.get_all_chunks(): 
        print(p.decode_chunk(ch)) 


