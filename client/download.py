from file import *
from utils import *
from MySocket import *

from shutil import copyfile
import json
import os

BLOCKSIZE = 1024
class User:
    def __init__(self):
        self.socket = None
    # mode :login register
    def login(self,username,passwd,mode):
        self.socket = Socket()
        request = mode + " " + username +' '+ passwd
        print(request)
        self.socket.send(request)
        data = self.socket.recv()
        info = json.load(data)
        if info['ack']==1:
            self.username = username
            self.cookie = info['cookie']
            return True
        return False


    # fielPath : where to store (maybe exist)
    # MD5 : which one to download
    # down_length: 本地已存在长度
    # isDownload: 是否正在下载
    def download(self,MD5,down_length,file_length,file_path):
        fd = open(file_path,'a+')    #create if not exist
        seq = fd.tell()          #get length

        print(seq,down_length)
        if(down_length != seq):
            os.remove(file_path)
        
        #断线重连
        for i in range (0,int(config['network']['reconnectLimit'])):
            try:
            # if True:
                self.socket = Socket()
                request = "download "+ MD5 + " " + str(seq) 
                print(request)
                self.socket.send(request)

                sql_update_isdownload = \
                    "update historyFile set is_download = '1' where file_path = '" + file_path +"'"
                db_exec(sql_update_isdownload)

                while down_length < file_length:
                    print(down_length," ",self.socket.status==Status.CONNECTED)
                    data = self.socket.recv(BLOCKSIZE)
                    fd.write(data)
                    down_length += len(data)
     
                    sql_update_down_length = \
                        "update historyFile set down_length = '" + str(down_length) + "' where file_path = '" + file_path + "'" 
                    # print(sql_update_down_length)
                    db_exec(sql_update_down_length)
                    break
            except Exception as e:
                print("FOR ERROR",e)
                self.socket = Socket()
                fd.close()

        if(self.socket.status == Status.CLOSED):
            sql_update_isdownload = \
                "update historyFile set is_download ='0' where file_path = '" + file_path +"'"
            db_exec(sql_update_isdownload)
        elif(down_length != file_length):
            raise Exception('文件长度对不上')
        elif(get_file_md5(file_path)!=MD5):
            raise Exception('文件MD5对不上')
        else:
            sql_update_isfinish = \
                "update historyFile set is_finish ='1' where file_path = " + file_path + "'"
            db_exec(sql_update_isfinish)


            

if __name__=='__main__':
    user = User()
    user.download("md5",0,100,'./d.txt')
    # open('/ttt/aaa.txt','ab+')
    # md5 = get_file_md5('F:/linux-3.10.tar.gz')
    # print(md5)
    # download('./b.txt','qqqq')