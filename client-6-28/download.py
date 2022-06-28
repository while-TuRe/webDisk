from file import *
from utils import *

from shutil import copyfile
import json

class User:
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

    def downloadBlock(self,fd,MD5,seq,length):
        socket = Socket()
        socket.send()

    # fielPath : where to store (maybe exist)
    # MD5 : which one to download
    def download(self,MD5,down_length,file_length,file_path):
        file_path ="/"
        length 
        fd = open(file_path,'ab+')    #create if not exist
        seq = fd.tell()          #get length

        if(down_length != seq):
            os.remove(file_path)
        target = localFiles.getByMD5(MD5)
        source = remoteFiles.getByMD5(MD5)
        # do not need to really download
        # if target:
        #     if get_file_md5(target['full_path'])==MD5:
        #         copyfile(target['full_path'], filePath)
        #         localFiles.append({'full_path':filePath,'MD5':MD5})
        #         return 
        #     elif target['length']!= seq:#local_files wrong
        #         localFiles.delete(target['full_path'])
        
        # send command
        self.socket = Socket(config['network']['ip'], config['network']['port'])
        reqest = "download "+ MD5 + " " + seq 
        print(request)
        socket.send(reqest)

        while target['length'] < source['length']:
            downloadBlock(fd, MD5, target['length'], BLOCKSIZE)
            self.socket
            target['length'] += BLOCKSIZE
            

if __name__=='__main__':
    open('/ttt/aaa.txt','ab+')
    # md5 = get_file_md5('F:/linux-3.10.tar.gz')
    # print(md5)
    # download('./b.txt','qqqq')