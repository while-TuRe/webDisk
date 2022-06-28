import json
import sqlite3
# LOCAL_KEYS = ['full_path','MD5','length','up_time']   #本地文件存储位置，包含下载了一半的，
# REMOTE_KEYS = ['full_path','MD5']           #
# UPLOADING_KEYS = ['full_path','MD5','length']
# class FileList:
#     def __init__(self,keys):
#         self.files = []
#         self.keys = keys
#     def loadFromJson(self,files_str):
#         self.files = json.loads(files_str)
#
#     def append(self,file_new):
#         #print(file_new)
#         # check type
#         for key in self.keys:
#             if(key not in file_new):
#                 raise TypeError("【MY ERROR】file parameter wrong: ",key)
#         #check exist
#         for file in self.files:
#             if file['full_path'] == file_new['full_path']:
#                 #print("PATH: ",file['full_path'],file_new['full_path'])
#                 return False
#         self.files.append(file_new)
#         return True
#
#     def delete(self,path):
#         # cnt = 0
#         self.files = [file for file in self.files if not file['full_path'].startswith(path)]
#         #不能变遍历边remove，会改变下标
#         #[filter(lambda file:~ file['full_path'].startswith(path), self.files)]
#         # for file in self.files:
#         #     print("PATH, ",file['full_path'],path)
#         #     if file['full_path'].startswith(path):
#         #         # self.files.remove(file)
#         #         cnt += 1
#         # return cnt
#
#     #get childrens recursively
#     # return file in {} mode
#     def getChildrenR(self,dir_path):
#         results = []
#         for file in self.files:
#             if file['full_path'].startswith(dir_path):
#                 results.append(file)
#         return results
#
#     #get children files and dirs
#     # return : child_files: filename with out path
#     #           child_dirs: dir name without path
#     def getChildren(self,dir_path):
#         child_files = []
#         child_dirs = []
#         if(dir_path[-1]!='/'):
#             dir_path = dir_path+'/'
#         for file in self.files:
#             if file['full_path'].startswith(dir_path):
#                 child_path = file['full_path'][len(dir_path):]
#
#                 #child files
#                 index=child_path.find('/')
#
#                 if(index==-1):#file
#                     child_files.append(child_path)
#                 else:#dir
#                     if child_path[0:index] not in child_dirs:
#                         child_dirs.append(child_path[0:index])
#         return child_files,child_dirs
#
#     def getByPath(self,path):
#         for file in self.files:
#             if file['full_path'] == path:
#                 return file
#         return None
#
#     def getByMD5(self,md5):
#         for file in self.files:
#             if file['MD5'] == md5:
#                 return file
#         return None
def db_exec(str):
    conn = sqlite3.connect('localFileSystem.db')
    c = conn.cursor()
    c.execute(str)
    results = c.fetchall()
    res=[]
    for row in results:
        # print(row)
        res.append(row)
    conn.commit()
    return res
class fileDb:
    def splite_path_file(self,str):
        index=-1
        if str[-1]!='/':
            for i in range(len(str)):
                if str[(len(str)-1-i)]=='/':
                    index=len(str)-1-i
                    break
            return [str[:index+1],str[index+1:]]
        else:
            for i in range(len(str)):
                if str[(len(str)-2-i)]=='/':
                    index=len(str)-2-i
                    break
            return [str[:index+1],str[index+1:-1]]
    def init_by_json_str(self,json_str):
        db_exec('drop table if exists userFile')
        sql = """create table userFile
        (
            uf_id INTEGER PRIMARY KEY AUTOINCREMENT,
            uf_md5 char(32),
            uf_path varchar(10000) not null,
            uf_file_name varchar(10000),
            uf_length varchar(100),
            uf_file_type varchar(10000),
            uf_up_time datetime not null,
            constraint unique_path Unique(uf_path,uf_file_name)
        )
        """
        db_exec(sql)

        fj=json.loads(json_str)
        for f in fj:
            sql="insert into userFile(uf_md5,uf_path,uf_file_name,uf_file_type,uf_length,uf_up_time) values ('{}','{}','{}','{}','{}','{}')"
            temp=self.splite_path_file(f['full_path'])

            if f['full_path'][-1]=='/':
                ftype='文件夹'
            elif '.'in temp[1]:
                ftype=temp[1].split('.')[1]
            else:
                ftype=''
            s=(sql.format(f['MD5'],temp[0],temp[1],ftype,f['length'],f['up_time']))
            print(s)
            db_exec(s)
    def get_file_list_by_dir(self,dir):
        res=db_exec("select uf_file_name,uf_up_time,uf_file_type,uf_length,uf_md5 from userFile where uf_path='"+dir+"'")
        # print(res)
        return res
    def get_parent_dir(self,dir):
        if dir=='/':
            return '/'
        # print(dir)
        for i in range(len(dir)-2,-1,-1):
            if dir[i]=='/':
                index=i
                break
        return dir[:index+1]
if __name__=="__main__":
    # db_exec("drop table historyFile")
    # sql = """create table historyFile
    # (
    #     uf_id INTEGER PRIMARY KEY AUTOINCREMENT,
    #     uf_md5 char(32),
    #     down_length integer not null,
    #     file_length integer not null,
    #     file_path varchar(10000) not null,
    #     is_finish varchar(1),
    #     is_download varchar(1)
    # )
    # """
    # db_exec(sql)
    sql = """
        insert into historyFile values
        (4,'md52',0,100,'./d.txt',0,0)
    """
    db_exec(sql)
    # jstr=open('files/loacl_files.json').read()
    # fb=fileDb()
    # fb.init_by_json_str(jstr)
    # print(fb.get_parent_dir('/'))
    # fb.init_by_json(jstr)
    # print(fb.get_file_list_by_dir('/AA'))
    # res=db_exec('select  from userFile')
    # print(res)
    # path='/aa/'
    # print(fb.splite_path_file(path))
    # print(path.split('/'))
    # print(path.find('/'))
    # sql="""
    # create table if not exists userFile
    # (
    #     uf_id INTEGER PRIMARY KEY AUTOINCREMENT,
    #     uf_md5 char(32),
    #     uf_path varchar(10000) not null,
    #     uf_file_name varchar(10000),
    #     uf_up_time datetime not null
    # )
    # """
    # db_exec(sql)
    # files_str = '[{"full_path": "/", "MD5":"ldsfhesiofhsfioefhel"},\
    #     { "full_path": "/dir1/filename", "MD5":"kukyjgfdvefhel"},\
    #     { "full_path": "/dir1/dir11/a", "MD5":"kukyjgfdvefhel"}]'
    # files_str=open('files/loacl_files.json')
    #
    # localFiles = FileList(REMOTE_KEYS)
    # localFiles.loadFromJson(files_str)
    # current_files,current_dirs = localFiles.getChildren("/dir1/")
    # print("test getchildren",current_files,current_dirs)
    #
    # child_files =localFiles.getChildrenR("/dir1/dir11")
    # print("test getchildrenR",child_files)
    #
    # newF = child_files[0].copy()
    # newF['full_path']="/dir1/dir11/b"
    # ret = localFiles.append(newF)
    # child_files =localFiles.getChildrenR("/dir1")
    # print("test append",ret,child_files,"\n")
    #
    # print('\n',localFiles.files,'\n')
    #
    # cnt = localFiles.delete("/dir1/dir11")
    # print("test delete")
    # print('\n',localFiles.files,'\n')
    #
    # print("test getByMD5",localFiles.getByMD5('ldsfhesiofhsfioefhel'),'\n')
    # print("test getByMD5",localFiles.getByPath('/dir1/filename'),'\n')