#!/usr/bin/env python3

import tlay2_monitor
import tlay2_v86_monitor
import struct

class pci():
    def __init__(self):
        self.v86monitor = tlay2_v86_monitor.tlay2_v86_monitor()
        self.monitor = tlay2_monitor.tlay2_monitor()  
    
    def pcisupport(self):
        regs = self.v86monitor.get_emptyregs()
        regs["ax"] = 0xB101
        self.v86monitor.vm86_int_call(0x1A, regs)
        info = {}
        info["supported"] = (regs["ax"]&0xFF00 == 0x0000) and (regs["dx"] == 0x20494350)
        if info["supported"]:
            info["mode1"] = bool(regs["ax"]&0x0001)
            info["mode2"] = bool(regs["ax"]&0x0002)
            info["maxbus"] = regs["cx"]&0xff
        return info

    def getreg(self,bus,slot,func,offset):
        address = (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | (1<<31)
        self.monitor.outl(0xCF8, address)
        return self.monitor.inl(0xCFC)
    
    def bardecode(self,barval):
        bar = {}
        bar["type"] = "io" if (barval&0x01) else "memory"
        if bar["type"] == "io":
            bar["address"] = barval & 0xFFFFFFFC
        elif bar["type"] == "memory":
            bar["address"] = barval & 0xFFFFFFF0
            bar["prefechable"] = bool(barval & 0x8)
            bar["typemem"] = (barval & 0x6)>>1
        bar["addresshex"] = f"{bar['address']:#0{10}x}"
        return bar
    
    def getheader(self,bus,slot,func):
        info = {}
        devvend = self.getreg(bus, slot, func, 0)
        info["vid"] = f"{(devvend & 0xffff):#0{6}x}"
        info["pid"] = f"{(devvend >> 16):#0{6}x}"
        statcmd = self.getreg(bus, slot, func, 4)
        info["status"] = statcmd >> 16
        info["command"] = statcmd & 0xfff
        classsubifid = self.getreg(bus, slot, func, 8)
        info["class"] = (classsubifid >> 24) & 0xff
        info["subclass"] = (classsubifid >> 16) & 0xff
        info["progif"] = (classsubifid >> 8) & 0xff
        info["revid"] = (classsubifid >> 0) & 0xff
        bisthederlatencycache=self.getreg(bus, slot, func, 0xC)
        info["bist"] = (bisthederlatencycache >> 24) & 0xff        
        info["headertype"] = (bisthederlatencycache >> 16) & 0xff        
        info["latency"] = (bisthederlatencycache >> 8) & 0xff        
        info["cacheline"] = (bisthederlatencycache >> 0) & 0xff        
        if info["headertype"]&0x7f == 0x00:
            info["bar0"] = self.bardecode(self.getreg(bus, slot, func, 0x10))
            info["bar1"] = self.bardecode(self.getreg(bus, slot, func, 0x14))
            info["bar2"] = self.bardecode(self.getreg(bus, slot, func, 0x18))
            info["bar3"] = self.bardecode(self.getreg(bus, slot, func, 0x1C))
            info["bar4"] = self.bardecode(self.getreg(bus, slot, func, 0x20))
            info["bar5"] = self.bardecode(self.getreg(bus, slot, func, 0x24))
            info["cisptr"] = self.getreg(bus, slot, func, 0x28)
            info["cisptrhex"] = f"{info['cisptr']:#0{10}x}"
            subidsubvid = self.getreg(bus, slot, func, 0x2C)
            info["subid"] = subidsubvid >> 16
            info["subvid"] = subidsubvid & 0xffff
            info["exprom"] = self.getreg(bus, slot, func, 0x30)
            info["expromhex"] = f"{info['exprom']:#0{10}x}"
            info["capsptr"] = self.getreg(bus, slot, func, 0x34) & 0xff
            latgrantpinirql = self.getreg(bus, slot, func, 0x3C)
            info["maxlatency"] = (latgrantpinirql >> 24) & 0xff
            info["mingrant"] = (latgrantpinirql >> 16) & 0xff
            info["intpin"] = (latgrantpinirql >> 8) & 0xff
            info["intline"] = (latgrantpinirql >> 0) & 0xff
        elif info["headertype"]&0x7f == 0x01:
            info["bar0"] = self.bardecode(self.getreg(bus, slot, func, 0x10))
            info["bar1"] = self.bardecode(self.getreg(bus, slot, func, 0x14))          
            timsbussbuspbus= self.getreg(bus, slot, func, 0x18)
            info["seclattim"] = (timsbussbuspbus >> 24) & 0xff
            info["subbus"] = (timsbussbuspbus >> 16) & 0xff
            info["secbus"] = (timsbussbuspbus >> 8) & 0xff
            info["pribus"] = (timsbussbuspbus >> 0) & 0xff
            secstatiolimiob = self.getreg(bus, slot, func, 0x1C)
            iolimiobhigh = self.getreg(bus, slot, func, 0x30)
            info["secstat"] = secstatiolimiob >> 16
            info["iolim"] = (secstatiolimiob >> 8) & 0xff | ((iolimiobhigh >> 8) & 0xffff00)
            info["iolimhex"] = f"{info['iolim']:#0{8}x}"
            info["iobase"] = (secstatiolimiob >> 0) & 0xff | ((iolimiobhigh << 8) & 0xffff00)
            info["iobasehex"] = f"{info['iobase']:#0{8}x}"
            memlimmembase = self.getreg(bus, slot, func, 0x20)
            info["memlim"] = memlimmembase >> 16
            info["membase"] = memlimmembase & 0xffff
            prefetchlimbase = self.getreg(bus, slot, func, 0x24)
            prefetchbasehigh = self.getreg(bus, slot, func, 0x28)
            prefetchlimhigh = self.getreg(bus, slot, func, 0x2C)
            info["prefetchbase"] = prefetchlimbase & 0xffff | (prefetchbasehigh << 16)
            info["prefetchbasehex"] = f"{info['prefetchbase']:#0{14}x}"
            info["prefetchlim"] = (prefetchlimbase >> 16) | (prefetchlimhigh << 16)
            info["prefetchlimhex"] = f"{info['prefetchlim']:#0{14}x}"
            info["exprom"] = self.getreg(bus, slot, func, 0x38)
            info["expromhex"] = f"{info['exprom']:#0{10}x}"
            info["capsptr"] = self.getreg(bus, slot, func, 0x34) & 0xff
            brctrlintpinintline = self.getreg(bus, slot, func, 0x3C)
            info["brctlr"] = (brctrlintpinintline >> 16)
            info["intpin"] = (brctrlintpinintline >> 8) & 0xff
            info["intline"] = (brctrlintpinintline >> 0) & 0xff
        elif info["headertype"]&0x7f == 0x02:
            info["cbsockbaseaddr"] = self.getreg(bus, slot, func, 0x10)
            secstatcapslist = self.getreg(bus, slot, func, 0x14)
            info["secstat"] = secstatcapslist >> 16
            info["offsetcapslist"] = (secstatcapslist & 0xff)
            latsubcbbpcib = self.getreg(bus, slot, func, 0x18)
            info["cblat"] = latsubcbbpcib >> 24
            info["subbus"] = (latsubcbbpcib >> 16) & 0xff
            info["cbbusn"] = (latsubcbbpcib >> 8) & 0xff
            info["pcibusn"] = (latsubcbbpcib >> 0) & 0xff
            info["membaseaddr0"] = self.getreg(bus, slot, func, 0x1C)
            info["membaseaddr0hex"] = f"{info['membaseaddr0']:#0{10}x}"
            info["memlimaddr0"] = self.getreg(bus, slot, func, 0x20)
            info["memlimaddr0hex"] = f"{info['memlimaddr0']:#0{10}x}"
            info["membaseaddr1"] = self.getreg(bus, slot, func, 0x24)
            info["membaseaddr1hex"] = f"{info['membaseaddr1']:#0{10}x}"
            info["memlimaddr1"] = self.getreg(bus, slot, func, 0x28)
            info["memlimaddr1hex"] = f"{info['memlimaddr1']:#0{10}x}"
            info["iobaseaddr0"] = self.getreg(bus, slot, func, 0x2C)
            info["iobaseaddr0hex"] = f"{info['iobaseaddr0']:#0{10}x}"
            info["iolimaddr0"] = self.getreg(bus, slot, func, 0x30)
            info["iolimaddr0hex"] = f"{info['iolimaddr0']:#0{10}x}"
            info["iobaseaddr1"] = self.getreg(bus, slot, func, 0x34)
            info["iobaseaddr1hex"] = f"{info['iobaseaddr1']:#0{10}x}"
            info["iolimaddr1"] = self.getreg(bus, slot, func, 0x38)
            info["iolimaddr1hex"] = f"{info['iolimaddr1']:#0{10}x}"
            brctrlintpinintline = self.getreg(bus, slot, func, 0x3C)
            info["brctlr"] = (brctrlintpinintline >> 16)
            info["intpin"] = (brctrlintpinintline >> 8) & 0xff
            info["intline"] = (brctrlintpinintline >> 0) & 0xff
            subidsubvid = self.getreg(bus, slot, func, 0x40)
            info["subid"] = subidsubvid >> 16
            info["subvid"] = subidsubvid & 0xffff
            info["legacy16bitpcbaseaddr"] = self.getreg(bus, slot, func, 0x44)
        return info
            
if __name__ == "__main__":
    p=pci()
    support=p.pcisupport()
    print(support)
    devices = {}
    for bus in range(support["maxbus"]+1):
        for device in range(32):
            hdr = p.getheader(bus, device, 0)
            if hdr["vid"] == '0xffff':
                continue
            devices[(bus,device,0)] = hdr
            if hdr["headertype"] & 0x80:
                for function in range(1,8):
                    hdr = p.getheader(bus, device, function)
                    if hdr["vid"] == '0xffff':
                        break
                    devices[(bus,device,function)] = hdr
    import yaml
    print(yaml.dump(devices,default_flow_style=False))
                    
    
    