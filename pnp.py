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
    def pnp_decode(self,node):
        ret = {}
        ret["node"] = node[2]
        id = struct.unpack(">I",node[3:7])[0]
        def toascii(num):
            return chr(((num)&0x1F)+0x40)
        ret["id"] = toascii(id>>26) + toascii(id>>21) + toascii(id>>16) + toascii(id>>10) + " " + format(id&0x0f,"#01x")
        ret['devtype'] = struct.unpack("<I",b"\x00" + node[7:10])[0]
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
        ret["VID"] = toascii(vid>>10) + toascii(vid>>5) + toascii(vid>>0) + " " + format(struct.unpack(">H",node[14:16])[0],"#4x")
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
