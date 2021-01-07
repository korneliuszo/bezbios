
import tlay2_client

class tlay2_monitor():
    def __init__(self):
        self.conn = tlay2_client.Tlay2_msg(1)
    def ping(self,data):
        return self.conn.msg(bytes([0])+data)[1:]
