#include "FDMgr.h"
#include "util.h"

FDMgr fd_mgr;

thread::id FDMgr::getThreadId()
{
    return get_id();
}

queue<int> *FDMgr::getFdQueue(thread::id &t_id)
{
    map<thread::id, queue<int>>::iterator iter = fds.find(t_id);
    if (iter == fds.end())
    {
        return nullptr;
    }
    return &(iter->second);
}

void FDMgr::addFd(thread::id &&t_id, int fd)
{
    queue<int> *q = getFdQueue(t_id);
    if (q == nullptr)
    {
        fds[t_id] = queue<int>();
        q = &(fds[t_id]);
        queue_mtx[t_id] = new (nothrow) mutex();
        if (queue_mtx[t_id] == NULL)
        {
            writeLog(getNowTime(), " create mutex error");
            exit(-1);
        }
    }
    queue_mtx[t_id]->lock();
    q->push(fd);
    queue_mtx[t_id]->unlock();
}

int FDMgr::peek()
{
    thread::id id = getThreadId();
    queue<int> *q = getFdQueue(id);
    if (q == nullptr)
    {
        writeLog(getNowTime(), " this thread(id:", id, ") has no fd queue");
        return -1;
    }
    int fd;
    queue_mtx[id]->lock();
    if (q->size() > 0)
        fd = q->front();
    else
        fd = -1;
    queue_mtx[id]->unlock();
    return fd;
}

int FDMgr::popFront()
{
    thread::id id = getThreadId();
    queue<int> *q = getFdQueue(id);
    if (q == nullptr)
    {
        writeLog(getNowTime(), " this thread(id:", id, ") has no fd queue");
        return -1;
    }
    int fd;
    queue_mtx[id]->lock();
    if (q->size() > 0)
    {
        fd = q->front();
        q->pop();
    }
    else
        fd = -1;
    queue_mtx[id]->unlock();
    return fd;
}

int FDMgr::getFdNum()
{
    thread::id id = getThreadId();
    queue<int> *q = getFdQueue(id);
    if (q == nullptr)
    {
        writeLog(getNowTime(), " this thread(id:", id, ") has no fd queue");
        return -1;
    }

    int num;
    queue_mtx[id]->lock();
    if (q->size() > 0)
        num = q->size();
    else
        num = -1;
    queue_mtx[id]->unlock();

    return num;
}
