#!/usr/bin/env python3

import tlay2_client
import struct

class pnp():
    def __init__(self):
        self.conn = tlay2_client.Tlay2_msg(3)
    def pnp_info(self):
        ret = self.conn.msg(bytes([0]))
        decoded = struct.unpack("<HBH",ret)
        return {"retcode" : hex(decoded[0]),
                "no_nodes" : decoded[1],
                "max_size" : decoded[2]}
    def pnp_get_node(self,node_no):
        ret = self.conn.msg(bytes([1,node_no]))
        retcode = struct.unpack("<HB",ret[0:3])
        return {"retcode" : hex(retcode[0]),
                "nextnode" : retcode[1],
                "raw" : ret[3:]}
    def isares(self,resources):
        ret = []
        pos = 0
        while resources[pos] != 0x79:
            entry = {}
            is_small = not bool(resources[pos]&0x80)
            entry["is_small"] = is_small
            if is_small:
                entry["tag"] = (resources[pos]>>3)&0x0F
                entry["len"] = (resources[pos]&0x07)
                if entry["tag"] == 1:
                    entry["pnpversion"] = (resources[pos+1]>>4)*10 + (resources[pos+1] & 0x0f)
                    entry["venspec"] = resources[pos+2]
                elif entry["tag"] == 4:
                    entry["irqmask"] = format(struct.unpack("<H",resources[pos+1:pos+3])[0],"#06x")
                    if entry["len"] == 3:
                        entry["irqlevel"] = resources[pos+3]
                elif entry["tag"] == 5:
                    entry["dmachannel"] = resources[pos+1]
                    entry["dmatype"] = resources[pos+2]
                elif entry["tag"] == 8:
                    entry["isalong"] = bool(resources[pos+1]&0x01)
                    entry["rangemin"] = hex(resources[pos+2] + (resources[pos+3]<<8))
                    entry["rangemax"] = hex(resources[pos+4] + (resources[pos+5]<<8))
                    entry["basealign"] = hex(resources[pos+6])
                    entry["rangelen"] = hex(resources[pos+7])
                else:
                    raise Exception("Not known type: " + str(entry["tag"]))
                pos += entry["len"] + 1
            else:
                entry["tag"] = (resources[pos])&0x7F
                entry["len"] = resources[pos+1]+(resources[pos+2]<<8)
                if entry["tag"] == 6 or entry["tag"] == 5:
                    entry["exprom"] =bool(resources[pos+3]&0x40)
                    entry["shadowable"] =bool(resources[pos+3]&0x20)
                    entry["memctrl"] ={
                        0: "8bit",
                        1: "16bit",
                        2: "8&16bit",
                        3: "32bitonly"}[(resources[pos+3]>>3)&0x03]
                    entry["suptype"] =bool(resources[pos+3]&0x04)
                    entry["cache"] =bool(resources[pos+3]&0x02)
                    entry["writable"] =bool(resources[pos+3]&0x01)
                    if entry["tag"] == 5:
                        entry["rangemin"] = hex(resources[pos+4] + (resources[pos+5]<<8) + (resources[pos+6]<<16) + (resources[pos+7]<<24))
                        entry["rangemax"] = hex(resources[pos+8] + (resources[pos+9]<<8) + (resources[pos+10]<<16) + (resources[pos+11]<<24))
                        entry["basealign"] = hex(resources[pos+12] + (resources[pos+13]<<8) + (resources[pos+14]<<16) + (resources[pos+15]<<24))
                        entry["rangelen"] = hex(resources[pos+16] + (resources[pos+17]<<8) + (resources[pos+18]<<16) + (resources[pos+19]<<24))
                    elif entry["tag"] == 6:
                        entry["rangebase"] = hex(resources[pos+4] + (resources[pos+5]<<8) + (resources[pos+6]<<16) + (resources[pos+7]<<24))
                        entry["rangelen"] = hex(resources[pos+8] + (resources[pos+9]<<8) + (resources[pos+10]<<16) + (resources[pos+11]<<24))
                else:
                    raise Exception("Not known type: " + str(entry["tag"]))
                pos += entry["len"] + 3
            ret.append(entry)
        return ret
    def pnp_decode(self,node):
        ret = {}
        ret["node"] = node[2]
        id = struct.unpack(">I",node[3:7])[0]
        def toascii(num):
            return chr(((num)&0x1F)+0x40)
        ret["id"] = toascii(id>>26) + toascii(id>>21) + toascii(id>>16) + " " + format((id>>4)&0xfff,"#05x") + " " + format(id&0x03,"#04x")
        ret['devtypebase'] = node[7]
        ret['devtypesub'] = node[8]
        ret['devtypeif'] = node[9]
        type= struct.unpack("<H",node[10:12])[0]
        ret['configurable'] = {
            0: "static",
            1: "dynamic",
            2: "reserved",
            3: "only dynamic"
            }[(type>>7)&0x3]
        ret["removable"] = bool((type>>6)&0x1)
        ret["docking"] = bool((type>>5)&0x1)
        ret["IPL"] = bool((type>>4)&0x1)
        ret["primary input"] = bool((type>>3)&0x1)
        ret["primary output"] = bool((type>>2)&0x1)
        ret["configurable"] = not bool((type>>1)&0x1)
        ret["disablalbe"] = bool((type>>0)&0x1)
        vid = struct.unpack(">H",node[12:14])[0]
        ret["isares"] = node[12:].hex(" ")
        ret["isaresdecoded"] = self.isares(node[12:])
        return ret
        
if __name__ == "__main__":
    p=pnp()
    print(p.pnp_info())
    a=p.pnp_get_node(0)
    import yaml
    while a["nextnode"] != 0xff:
        print(a)
        print(yaml.dump(p.pnp_decode(a['raw']),default_flow_style=False))
        a=p.pnp_get_node(a["nextnode"])
