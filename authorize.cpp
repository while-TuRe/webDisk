# if 0

#include <sys/epoll.h>
#include<unistd.h>

#include "configor/json.hpp"
#include"util.h"
#include"FDMgr.h"


using namespace std;
using namespace configor;

class Authenticator
{
private:
    int epoll_fd;
    int fd_cnt;

public:
    Authenticator()
    {
        epoll_fd = epoll_create(epoll_size);
        if (epoll_fd < 0)
        {
            writeLog(getNowTime(), "create epoll error in download");
            return;
        }
        fd_cnt = 0;
    }

    ~Authenticator()
    {
        close(epoll_fd);
    }




};


void login() {
    // take out all fds in each round of service
    while (fd_mgr.getFdNum() > 0)
    {
        json j;
        j["ack"] = 1;
        j["cookie"] = "this is a cookie";
         int fd = fd_mgr.popFront();

    }
}


void registerAccount() {
    Authenticator authenticator;
    epoll_event events[epoll_size];
}


#endif