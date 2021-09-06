
import tlay2_client
import struct
import copy

class tlay2_v86_monitor():
    def __init__(self):
        self.conn = tlay2_client.Tlay2_msg(2)
    def get_scratchaddr(self):
        ret = self.conn.msg(bytes([0]))
        decoded = struct.unpack("<IHH",ret)
        return {"linear": decoded[0],
                "segment": decoded[1],
                "offset": decoded[2]}
    emptyregs = {
        "ax":0,"bx":0,"cx":0,"dx":0,
        "si":0,"di":0,"bp":0,"ef":0,
        "ds":0,"es":0,"fs":0,"gs":0}
    def get_emptyregs(self):
        return copy.copy(self.emptyregs)
    def vm86_int_call(self,irq,regs):
        cmd = bytes([1]) + struct.pack("<BLLLLLLLLLLL",
                irq,
                regs["ax"],
                regs["bx"],
                regs["cx"],
                regs["dx"],
                regs["si"],
                regs["di"],
                regs["bp"],
                regs["ds"],
                regs["es"],
                regs["fs"],
                regs["gs"])
        ret = self.conn.msg(cmd)
        keys = ("ax","bx","cx","dx","si","di","bp","eflags")
        values = struct.unpack("<LLLLLLLL",ret)
        d = dict(zip(keys,values))
        return regs.update(d)
        
        
        