import socket   
import sys
import time
from utils import *


class Socket:
    def __init__(self,ip,port):
        self.ip = ip
        self.port = port
        self.reconnect()
    def reconnect(self):
        for i in range(0,config['network']['reconnectLimit']):
            time.sleep(3)
            try:
                self.socket = socket.socket()   # 创建socket对象
                self.socket.connect((self.ip, self.port))   #建立连接
                log.write(getNowTime() + " No. " + i +" try to connect")
                break
            except Exception as e:
                log.write(getNowTime() +" connect failed")
                
    def send(self,data):
        try:
            self.socket.send(data)
        except Exception as e:
            log.write(getNowTime() + " connection closed")
            self.reconnect(self.ip, self.port)


if __name__ == '__main__':
    Socket(config['network']['ip'], config['network']['port'])
    localFiles = FileList(LOCAL_KEYS)
    remoteFiles = FileList(LOCAL_KEYS)
    downloadFiles = FileList(LOCAL_KEYS)
    # while True:
    #     ab=input('客户端发出：')
    #     if ab=='quit':
    #         c.close()                                               #关闭客户端连接
    #         sys.exit(0)
    #     else:
    #         c.send(ab.encode('utf-8'))                               #发送数据
    #         data=c.recv(1024)                                        #接收一个1024字节的数据
    #         print('收到：',data.decode('utf-8'))