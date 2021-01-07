
import tlay2_client

def ping(data,conn=None):
    if conn is None:
        conn = tlay2_client.Tlay2_msg(1)
    return conn.msg(bytes([0])+data)[1:]
