#!/usr/bin/env python3

import tlay2_monitor

import struct



if __name__ == "__main__":
    m  = tlay2_monitor.tlay2_monitor()
    for addr in range(0xE0000,0xFFFF0,16):
        candidate = m.getmem(addr,4)
        if struct.unpack("<I",candidate)[0] == 0x52495024:
            len = struct.unpack("<H",m.getmem(addr+6,2))[0]
            print("Found PIR Len:",len)
            p=m.getmem(addr,len)
            chk=0
            for b in p:
                chk+=b
            chk=chk%256
            print("checksum",chk) 
            if chk:
                continue
            print("PCI Interrupt Routing %u.%u present."%(
        p[5], p[4]))
            print("\tRouter Device: %02x:%02x.%1x"%(
        p[8], p[9]>>3, p[9] & 0x07));
        
            print("\tExclusive IRQs:",end=" ");
            exirq = struct.unpack("<H",p[10:12])[0];
            if not exirq:
                print(" None",end=" ")
            else:
                for i in range(16):
                    if (1<<i)&exirq:
                        print(i,end=" ")
            print()
                  
            if struct.unpack("<I",p[12:16])[0] != 0:
                print("\tCompatible Router: %04x:%04x"%(
            struct.unpack("<H",p[12:14])[0], struct.unpack("<H",p[14:16])[0]));
            
            minp = struct.unpack("<I",p[16:20])[0]
            if minp:
                print("\tMiniport Data: 0x%08X"%(minp,))
            
            n = (len - 32) // 16;
            for i in range(n):
                d=p[32+16*i:48+16*i]
                dec = struct.unpack("<BBBHBHBHBHBB",d)
                print("\tDevice: %02x:%02x,"%( dec[0], dec[1] >> 3),end=" ")
                if not dec[10]:
                    print(" on-board")
                else:
                    print(" slot %u"%(dec[10],));
                
                def prnt_link(letter,p):
                    print("\t\tINT%c#: Link 0x%02x, IRQ Bitmap"%( letter, p[0]),end=" ")
                    exirq = p[1];
                    if not exirq:
                        print(" None",end=" ")
                    else:
                        for i in range(16):
                            if (1<<i)&exirq:
                                print(i,end=" ")
                    print()
                prnt_link('A',dec[2:4])
                prnt_link('B',dec[4:6])
                prnt_link('C',dec[6:8])
                prnt_link('D',dec[8:10])

            break