import socket
import time
from utils import *
from enum import Enum
 
# 继承枚举类
class Status(Enum):
    CLOSED=0
    CONNECTED=1

class Socket:
    def __init__(self):
        self.ip = config['network']['ip']
        self.port = int(config['network']['port'])
        self.status = Status.CLOSED
        self.reconnect(pause=False)
    def reconnect(self,pause=True):
        for i in range(0,int(config['network']['reconnectLimit'])):
            if pause:
                time.sleep(3)
            log.write(getNowTime() + " No. " + str(i+1) + " try to connect\n")
            try:
                
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   # 创建socket对象
                self.socket.connect((self.ip, self.port))   #建立连接
                self.status = Status.CONNECTED
                break
            except Exception as e:
                # log.write(getNowTime() + " No. " + str(i) +" connect failed\n")
                print("exception in reconnect",e)
                self.status = Status.CLOSED
                
        

                
    def send(self,data):
        ret =0
        try:
            ret = self.socket.send(data.encode('utf-8'))
        except Exception as e:
            # log.write(getNowTime() + " No. " + str(i) + " connection closed\n")
            print("e in send",e)
            self.reconnect()
        finally:
            return ret
    
    def recv(self,recv):
        try:
            data = self.socket.recv().decode('utf-8')
        except Exception as e:
            print("exception in recv",e)
            log.write(getNowTime() + " connection closed")
            self.reconnect()
        finally:
            return data.decode('utf-8')

if __name__ == '__main__':
    log.write("-----------------restart--------------------")
    print("Mysocket.py")
    s = Socket()
    i=0
    while s.status==Status.CONNECTED:
        info = str(i)*100
        print("send", i)
        i+=1
        time.sleep(1)
        print("send return",s.send(info.encode('utf-8')))