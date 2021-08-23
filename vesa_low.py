import tlay2_monitor
import tlay2_v86_monitor
import struct
import itertools

class vesa():
    def __init__(self):
        self.v86monitor = tlay2_v86_monitor.tlay2_v86_monitor()
        self.monitor = tlay2_monitor.tlay2_monitor()
    
    scratchpad_int = None
    def scratchpad(self):
        if self.scratchpad_int is None:
            self.scratchpad_int = self.v86monitor.get_scratchaddr()
        return self.scratchpad_int
    
    def vbeFarPtr(self,addr):
        return (addr >> (16-4)) | (addr & 0xffff)
    
    def vbe_modes(self,saddr):
        for chnum in itertools.count(start=0):
            chunk = self.monitor.getmem(chnum*128+saddr,128)
            for i in range(0, len(chunk), 2):
                val = struct.unpack("<H",chunk[i:i+2])[0]
                if val == 0xffff:
                    return
                else:
                    yield val
    
    
    vbe_info_int = None
    
    def vbe_info(self):
        if self.vbe_info_int is not None:
            return self.vbe_info_int
        regs=self.v86monitor.get_emptyregs()
        regs["ax"] = 0x4f00
        regs["es"] = self.scratchpad()["segment"]
        regs["di"] = self.scratchpad()["offset"]
        scratch = b"VBE2" + b'\0'*(512-4)
        self.monitor.putmem(self.scratchpad()["linear"],scratch)
        self.v86monitor.vm86_int_call(16,regs)
        if regs["ax"] != 0x4f:
            raise Exception("VBE not found")
        scratch = self.monitor.getmem(self.scratchpad()["linear"],512)
        keys = ("sgnature","versionraw","oemstraddr","caps","vmodeaddr","memory")
        values = struct.unpack("<4sHIIIH",scratch[:20])
        d = dict(zip(keys,values))
        d["oemstr"]=self.monitor.readstring(self.vbeFarPtr(d["oemstraddr"]))
        ver=d["versionraw"]
        d["version"] = f"{ver>>8:d}.{ver&0xff:d}"
        d["modes"] = tuple(self.vbe_modes(self.vbeFarPtr(d["vmodeaddr"])))
        self.vbe_info_int = d
        return d
      
    def vbe_get_mode_info(self,mode):
        if not mode in self.vbe_info()["modes"]:
            raise Exception("Mode not supported")
        regs=self.v86monitor.get_emptyregs()
        regs["ax"] = 0x4f01
        regs["cx"] = mode
        regs["es"] = self.scratchpad()["segment"]
        regs["di"] = self.scratchpad()["offset"]
        self.v86monitor.vm86_int_call(16,regs)
        if regs["ax"] != 0x4f:
            raise Exception("VBE not found")
        scratch = self.monitor.getmem(self.scratchpad()["linear"],256)
        
        keys = ("ModeAttributes",
                "WinAAttributes",
                "WinBAttributes",
                "WinGranularity",
                "WinSize",
                "WinASegment",
                "WinBSegment",
                "WinFuncPtr",
                "BytesPerScanLine",
                "XResolution",
                "YResolution",
                "XCharSize",
                "YCharSize",
                "NumberOfPlanes",
                "BitsPerPixel",
                "NumberOfBanks",
                "MemoryModel",
                "BankSize",
                "NumberOfImagePages",
                "Reserved",
                "RedMaskSize",
                "RedFieldPosition",
                "GreenMaskSize",
                "GreenFieldPosition",
                "BlueMaskSize",
                "BlueFieldPosition",
                "RsvdMaskSize",
                "DirectColorModeInfo"
                )
        values = struct.unpack("<HBBHHHHIHHHBBBBBBBBBBBBBBBBB",scratch[:39])
        d = dict(zip(keys,values))
        return d
        
    modeinfo = None
    def set_video_mode(self,mode):    
        if not mode in self.vbe_info()["modes"]:
            raise Exception("Mode not supported")
        self.modeinfo = self.vbe_get_mode_info(mode)
        regs=self.v86monitor.get_emptyregs()
        regs["ax"] = 0x4f02
        regs["bx"] = mode
        self.v86monitor.vm86_int_call(16,regs)
        if regs["ax"] != 0x4f:
            raise Exception("VBE not found")
        
    def move_window(self,slice):
        regs=self.v86monitor.get_emptyregs()
        regs["ax"] = 0x4f05
        regs["bx"] = 0
        regs["dx"] = slice
        self.v86monitor.vm86_int_call(16,regs)
        if regs["ax"] != 0x4f:
            raise Exception("VBE not found")
 
    def putpixels(self,pixels):
        baseaddr = self.modeinfo['WinASegment'] <<4;
        winsize = self.modeinfo['WinSize'] * 1024
        wingran = self.modeinfo['WinGranularity'] * 1024
        bpixels = bytes(pixels)
        for i in range(0,len(pixels),winsize):
            self.move_window(i//wingran)
            self.monitor.putmem(baseaddr,bpixels[i:i+winsize])
        
    def setpalette(self,palette):
        regs=self.v86monitor.get_emptyregs()
        regs["ax"] = 0x1012
        regs["bx"] = 0
        regs["cx"] = len(palette)//3
        regs["es"] = self.scratchpad()["segment"]
        regs["dx"] = self.scratchpad()["offset"]
        self.monitor.putmem(self.scratchpad()["linear"],palette)
        self.v86monitor.vm86_int_call(16,regs)
                
        
                