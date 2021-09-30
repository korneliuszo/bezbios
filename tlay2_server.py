#!/usr/bin/env python3

import sys
import serial
import crcmod
import threading
import socket
import os

ser = serial.Serial(sys.argv[1], 38400)

crc8 = crcmod.predefined.mkCrcFun('crc-8')

connections = {}

my_mutex = threading.Event()

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("127.0.0.1",12346))

s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s2.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
s2.connect(("127.255.255.255",12347))

def udprecv():
    global connections
    global my_mutex
    curid = 1
    while True:
        data, addr = s.recvfrom(1024)
        #print("Sending:",data)
        buff=b''
        data = bytes([curid]) + data
        data += bytes([crc8(data)])
        connections[curid]= addr
        curid+=1
        if curid == 256:
            curid = 1
        for byte in data:
            if byte == 0x0a or byte == 0xdc:
                buff+=bytes([0xdc])
                byte ^= 0x80
            buff+=bytes([byte])
        buff+=b'\n'
        my_mutex.wait(1.5)
        my_mutex.clear()
        ser.write(buff)
    

t1 = threading.Thread(target = udprecv)

t1.start()

buff=b""
while True:
    c= ser.read()
    print(c)
    if c == b'\n':
        if len(buff) < 3:
            print("Too short packet",buff)
        elif crc8(buff) != 0:
            print("CRC ERROR")
        else:
            #print("Recv:",buff[1:-1])
            if buff[0] == 0:
                s2.send(buff[1:-1])
            else:
                my_mutex.set()
                s.sendto(buff[1:-1],connections[buff[0]])
        buff = b""
        continue
    if c == b"\xdc":
        c = bytes([(ser.read()[0] ^ 0x80)])
    buff+=c