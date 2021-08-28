#!/usr/bin/env python3

import tlay2_client
import tlay2_monitor
from elftools.elf.elffile import ELFFile
import itertools
import sys

conn = tlay2_client.Tlay2_out(0)
monitor = tlay2_monitor.tlay2_monitor()

while True:
    packet = conn.recv()
    if packet == b'LOADER_WORKING!':
        elf=ELFFile(open(sys.argv[1],"rb"))
        for i in itertools.count(0):
            segment=elf.get_segment(i)
            if segment.header['p_type'] == 'PT_NULL':
                break
            if segment.header['p_type'] == 'PT_LOAD':
                monitor.putmem(segment.header['p_paddr'], segment.data())
        monitor.callraw(elf.header['e_entry'])