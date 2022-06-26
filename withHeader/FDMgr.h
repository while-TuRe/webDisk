#include <map>
#include <queue>
#include <mutex>
#include <thread>
#include<unordered_map>
#include<condition_variable>

#include "util.h"

using namespace std;
using namespace std::this_thread;

class FDMgr
{
private:
    map<thread::id, queue<int>> fds;
    map<thread::id, mutex *> queue_mtx;

    unordered_map<thread::id, tuple<condition_variable *, mutex *>> condition_lock;

    thread::id getThreadId();

    queue<int> *getFdQueue(thread::id &t_id);

    void addFd(thread::id &&t_id, int fd);

    mutex &getMutex();

    tuple<condition_variable *, mutex *> &getLockTuple();

public:
    friend class WebFrame;

    int peek();

    int popFront();

    int getFdNum();
};
