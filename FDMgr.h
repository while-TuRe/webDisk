#include <map>
#include <queue>
#include <mutex>
#include <thread>

extern FDMgr fd_mgr;

using namespace std;
using namespace std::this_thread;

class FDMgr
{
private:
    map<thread::id, queue<int>> fds;
    map<thread::id, mutex *> queue_mtx;

    thread::id getThreadId();

    queue<int> *getFdQueue(thread::id &t_id);

    void addFd(thread::id &&t_id, int fd);

public:
    friend class WebFrame;

    int peek();

    int popFront();

    int getFdNum();
};