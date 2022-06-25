import json
LOCAL_KEYS = ['id','level','full_path','name','MD5']
class FileList:
    def __init__(self,keys):
        self.files = []
        self.keys = keys
    def loadFromJson(self,files_str):
        self.files = json.loads(files_str)

    def append(self,file_new):
        #print(file_new)
        # check type
        for key in self.keys:
            if(key not in file_new):
                raise TypeError("【MY ERROR】file parameter wrong: ",key)
        #check exist
        for file in self.files:
            if file['full_path'] == file_new['full_path']:
                #print("PATH: ",file['full_path'],file_new['full_path'])
                return False
        self.files.append(file_new)
        return True

    def delete(self,path):
        # cnt = 0
        self.files = [file for file in self.files if not file['full_path'].startswith(path)]
        #不能变遍历边remove，会改变下标
        #[filter(lambda file:~ file['full_path'].startswith(path), self.files)]
        # for file in self.files:
        #     print("PATH, ",file['full_path'],path)
        #     if file['full_path'].startswith(path):
        #         # self.files.remove(file)
        #         cnt += 1
        # return cnt

    #get childrens recursively 
    # return file in {} mode
    def getChildrenR(self,dir_path):
        results = []
        for file in self.files:
            if file['full_path'].startswith(dir_path):
                results.append(file)
        return results
    
    #get children files and dirs 
    # return : child_files: filename with out path
    #           child_dirs: dir name without path
    def getChildren(self,dir_path):
        child_files = []
        child_dirs = []
        if(dir_path[-1]!='/'):
            dir_path = dir_path+'/'
        for file in self.files:
            if file['full_path'].startswith(dir_path):
                child_path = file['full_path'][len(dir_path):]

                #child files
                index=child_path.find('/')

                if(index==-1):#file
                    child_files.append(child_path)
                else:#dir
                    child_dirs.append(child_path[0:index])
        return child_files,child_dirs

    def getByPath(self,path):
        for file in self.files:
            if file['full_path'] == path:
                return file
        return None
    
    def getByMD5(self,md5):
        for file in self.files:
            if file['MD5'] == md5:
                return file
        return None
    

if __name__=="__main__":
   
    files_str = '[{ "id": 1, "level": 1, "full_path": "/", "name": "" ,"MD5":"ldsfhesiofhsfioefhel"},\
        { "id": 2, "level": 3, "full_path": "/dir1/filename", "name": "filename" ,"MD5":"kukyjgfdvefhel"},\
        { "id": 3, "level": 4, "full_path": "/dir1/dir11/a", "name": "a" ,"MD5":"kukyjgfdvefhel"}]'
   
    localFiles = FileList(LOCAL_KEYS)
    localFiles.loadFromJson(files_str)
    current_files,current_dirs = localFiles.getChildren("/dir1/")
    print("test getchildren",current_files,current_dirs)
    
    child_files =localFiles.getChildrenR("/dir1/dir11")
    print("test getchildrenR",child_files)

    newF = child_files[0].copy()
    newF['full_path']="/dir1/dir11/b"
    ret = localFiles.append(newF)
    child_files =localFiles.getChildrenR("/dir1")
    print("test append",ret,child_files,"\n")

    print('\n',localFiles.files,'\n')

    cnt = localFiles.delete("/dir1/dir11")
    print("test delete")
    print('\n',localFiles.files,'\n')

    print("test getByMD5",localFiles.getByMD5('ldsfhesiofhsfioefhel'),'\n')
    print("test getByMD5",localFiles.getByPath('/dir1/filename'),'\n')