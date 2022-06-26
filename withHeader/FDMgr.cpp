#include <map>
#include <queue>
#include <mutex>
#include <thread>

#include "FDMgr.h"
#include "util.h"

using namespace std;
using namespace std::this_thread;

thread::id FDMgr::getThreadId()
{
    return get_id();
}

queue<int> *FDMgr::getFdQueue(thread::id &t_id)
{
    queue_mtx[t_id]->lock();
    map<thread::id, queue<int>>::iterator iter = fds.find(t_id);
    queue<int> *q;
    if (iter == fds.end())
        q = nullptr;
    else
        q = &(iter->second);
    queue_mtx[t_id]->lock();
    return q;
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

mutex &FDMgr::getMutex()
{
    thread::id id = get_id();
    if (condition_lock.find(id) == condition_lock.end())
    {
        condition_lock[id] = forward_as_tuple(new (nothrow) condition_variable(), new (nothrow) mutex());
        fds[id] = queue<int>();
    }
    mutex &m = *(get<1>(condition_lock[id]));
    return m;
}

tuple<condition_variable *, mutex *> &FDMgr::getLockTuple()
{
    return condition_lock[get_id()];
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
    num = q->size();
    queue_mtx[id]->unlock();

    return num;
}
