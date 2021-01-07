
import socket

class Tlay2_out():
    def __init__(self,fnaddr):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.s.bind(("127.255.255.255",12347))
        self.fnaddr = fnaddr
    def recv(self):
        while True:
            buff=self.s.recv(1024)
            if buff[0] == self.fnaddr:
                return buff[1:]
    
class Tlay2_msg():
    def __init__(self,fnaddr):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.s.connect(("127.0.0.1",12346))
        self.s.settimeout(0.5)
        self.fnaddr = bytes([fnaddr])
    def msgout(self,payload):
        self.s.send(self.fnaddr+payload)
    def msg(self,payload):
        self.msgout(payload)
        return self.s.recv(1024)[1:]
