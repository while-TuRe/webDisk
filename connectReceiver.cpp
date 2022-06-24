#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fstream>
#include <sys/epoll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <functional>
#include <unordered_map>
#include <thread>

using namespace std;

class WebFrame
{
private:
    int epoll_fd;
    unordered_map<string, thread> transaction_threads;
    unordered_map<string, function<void()>> url_map;

public:
    WebFrame()
    {
        epoll_fd = epoll_create(128);
        if (epoll_fd == -1)
        {
            cerr << "can't create connection epoll" << endl;
            exit(-1);
        }
    }

    ~WebFrame()
    {
        close(epoll_fd);
    }

    void route(string url, function<void()> func)
    {
        if (url_map.find(url) != url_map.end())
        {
            cerr << "url repeated" << endl;
            return;
        }

        auto wrapper = [&]
        {
            // something before calling
            func();
            // something after calling
        };

        url_map[url] = wrapper;
    }

    void run()
    {
    }
};
