#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <queue>
#include <unordered_map>
#include <functional>
#include <condition_variable>
#include <unistd.h>
#include <tuple>

#include "util.h"
using namespace std;
using namespace std::this_thread;

int cnt1 = 0;
int cnt2 = 0;

class Mgr
{
public:
    unordered_map<thread::id, tuple<condition_variable*, mutex*>> cond_lock;
    unordered_map<string, thread::id> url_map;
    unordered_map<thread::id, queue<int>> fds;
    mutex access_mgr_mtx;

    mutex &getMutex()
    {
        access_mgr_mtx.lock();
        thread::id id = get_id();
        if (cond_lock.find(id) == cond_lock.end())
        {
            cond_lock[id] = forward_as_tuple(new(nothrow) condition_variable(), new(nothrow) mutex());
            fds[id] = queue<int>();
        }
        mutex &m = *(get<1>(cond_lock[id]));
        cout << id << " getMutex " << endl;
        access_mgr_mtx.unlock();
        return m;
    }

    int getSize()
    {
        access_mgr_mtx.lock();
        thread::id id = get_id();
        int size = fds[id].size();
        access_mgr_mtx.unlock();
        return size;
    }

    int popFront()
    {
        access_mgr_mtx.lock();
        thread::id id = get_id();
        int fd = fds[get_id()].front();
        fds[get_id()].pop();
        cout << id << " pop " << fd << endl;
        access_mgr_mtx.unlock();
        return fd;
    }

    void push(string url, int i)
    {
        access_mgr_mtx.lock();
        thread::id &id = url_map[url];
        fds[id].push(i);
        cout << "add " << i << " to " << id << endl;
        get<0>(cond_lock[id])->notify_one();
        access_mgr_mtx.unlock();
    }

    void route(string url, function<void()> func)
    {
        function<void()> wrapper = [&]
        {
            while (true)
            {
                unique_lock<mutex> lock(getMutex());
                cout << get_id() << " may sleep" << endl;
                auto &t = cond_lock[get_id()];
                (get<0>(t))->wait(lock, [&]() -> bool
                               { return getSize() > 0; });
                cout << get_id() << " awake" << endl;
                func();
            }
        };

        thread t(wrapper);
        t.detach();
        url_map[url] = t.get_id();
        cout << "create thread id:" << t.get_id() << endl;
    }
};
Mgr mgr;

void reduce()
{
    while (mgr.getSize() > 0)
    {
        cout << get_id() << "  :" << mgr.popFront() << endl;
        sleep(1);
    }
}


int main()
{
    mgr.route("test1", reduce);
    mgr.route("test2", reduce);
    for (int i = 0;; i += 2)
    {
        sleep(1);
        mgr.push("test1", i);
        mgr.push("test2", i + 1);
    }
}