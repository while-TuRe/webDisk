import socket   
import sys
import time
from utils import *

if __name__ == '__main__':
    socket = Socket(config['network']['ip'], config['network']['port'])
    # localFiles = FileList(LOCAL_KEYS)
    # remoteFiles = FileList(LOCAL_KEYS)
    # downloadFiles = FileList(LOCAL_KEYS)
    i=32
    while True:
        info = char(i)*100
        print(info)
        time.sleep(0.1)
        socket.send(info)
        # ab=input('客户端发出：')
        # if ab=='quit':
        #     c.close()                                               #关闭客户端连接
        #     sys.exit(0)
        # else:
        #     c.send(ab.encode('utf-8'))                               #发送数据
        #     data=c.recv(1024)                                        #接收一个1024字节的数据
        #     print('收到：',data.decode('utf-8'))