#!/usr/bin/env python3


import tlay2_client

conn = tlay2_client.Tlay2_out(0)

while True:
    packet = conn.recv()
    print(packet)