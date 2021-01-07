
import tlay2_client
import struct

class tlay2_monitor():
    def __init__(self):
        self.conn = tlay2_client.Tlay2_msg(1)
    def ping(self,data):
        return self.conn.msg(bytes([0])+data)[1:]
    def outb(self,port,byte):
        self.conn.msg(struct.pack("<BHB",1,port,byte))
        return
    def inb(self,port):
        ret=self.conn.msg(struct.pack("<BH",2,port))
        return struct.unpack("<B",ret)[0]
    def putmem_page(self,addr,data):
        self.conn.msg(struct.pack("<BL",3,addr)+data)
        return
    def putmem(self,addr,data):
        [self.putmem_page(addr+i,data[i:i+512]) for i in range(0, len(data), 512)]
        return
    def getmem_page(self,addr,l):
        return self.conn.msg(struct.pack("<BLH",4,addr, l))
    def getmem(self,addr,l):
        return b"".join([self.getmem_page(addr+i,min(512,l-i)) for i in range(0, l, 512)])
    def outs(self,port,byte):
        self.conn.msg(struct.pack("<BHH",5,port,byte))
        return
    def ins(self,port):
        ret=self.conn.msg(struct.pack("<BH",6,port))
        return struct.unpack("<H",ret)[0]