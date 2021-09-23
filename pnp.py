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

if __name__ == "__main__":
    p=pnp()
    print(p.pnp_info())
