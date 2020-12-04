import socket, ssl
import optparse
import time
import sys

host = ''       # Symbolic name meaning the local host 
port = 50007    # Arbitrary non-privileged port

print("=============================================================================")
print("TCP Secure Server (IPv4 addressing mode)")
print("=============================================================================")
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

try:
    s.bind((host, port))
    s.listen(1)
except socket.error as msg:
    print("ERROR: ", msg)
    s.close()
    s = None

if s is None:
    sys.exit(1)

while 1:
    print("Listening on port: %d"%(port))
    data_len = 0
    try:
        conn, addr = s.accept()
        connstream = ssl.wrap_socket(conn,
                             server_side=True,
                             certfile="server.crt",
                             keyfile="server.key",
                             ca_certs='root_ca.crt',
                             cert_reqs=ssl.CERT_REQUIRED)
    except KeyboardInterrupt:
        print("Closing Connection")
        s.close()
        s = None
        sys.exit(1)

    print('Incoming connection accepted: ', addr)

    try:
        while 1:
                data = connstream.read(4096)
                print("Acknowledgement from TCP Client:", data.decode('utf-8'))
                print("")
                if (float(data) < 27.0):
                    print('0')
                    connstream.write(str('0').encode())
                elif ((float(data) > 27.0) and (float(data) < 30.0)):
                    print('1')
                    connstream.write(str('1').encode())
                elif (float(data) > 30.0):
                    print('2')
                    connstream.write(str('2').encode())

                # if not data: break
                
            
    except KeyboardInterrupt:
        conn.close()
        s.close()
        s = None
        print("\nConnection Closed")
        sys.exit(1)

# [] END OF FILE