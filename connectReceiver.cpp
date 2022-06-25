#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FDMgr.h"
#include "connectReceiver.h"
#include "const.h"
#include "util.h"

using namespace std;
using namespace std::this_thread;

void WebFrame::startService(int client_fd, string request_type)
{
    if (transaction_threads.find(request_type) == transaction_threads.end())
    {
        writeLog(getNowTime(), " request type not found, type is ", request_type);
        return;
    }
}

WebFrame::WebFrame()
{
    epoll_fd = epoll_create(epoll_size);
    if (epoll_fd == -1)
    {
        cerr << "can't create connection epoll" << endl;
        exit(-1);
    }
}

WebFrame::~WebFrame()
{
    close(epoll_fd);
}

void WebFrame::setNonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

void WebFrame::route(string url, function<void()> func)
{
    if (transaction_threads.find(url) != transaction_threads.end())
    {
        cerr << "url repeated" << endl;
        return;
    }

    function<void()> wrapper = [&]
    {
        // sleep
        while (true)
        {
            // awake()
            func();
            // sleep()
        }
    };

    thread t(wrapper);
    t.detach();
    transaction_threads[url] = t.get_id();
}

void WebFrame::init()
{
    accept_client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (accept_client_fd < 0)
    {
        cerr << "acquire server fd error, errno:" << strerror(errno) << endl;
        exit(-1);
    }

    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(local_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(accept_client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "bind server addr error, errno:" << strerror(errno) << endl;
        exit(-1);
    }

    if (listen(accept_client_fd, epoll_size) < 0)
    {
        cerr << "listen error, errno:" << strerror(errno) << endl;
        exit(-1);
    }

    // add accept_client_fd to epoll for accepting connection
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = accept_client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_client_fd, &event) < 0)
    {
        cerr << "add accept_client_fd to epoll error, errno:" << strerror(errno) << endl;
        exit(-1);
    }
}

void WebFrame::run()
{
    epoll_event events[epoll_size];
    // client will send request type once connected
    char request_type[request_type_max_size];
    while (true)
    {
        int readable_fd_num = epoll_wait(epoll_fd, events, epoll_size, -1);
        for (int i = 0; i < readable_fd_num; i++)
        {
            // client comes to connect
            if (events[i].data.fd = accept_client_fd)
            {
                sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int client_fd = accept(accept_client_fd, reinterpret_cast<sockaddr *>(&client_addr), &len);
                if (client_fd < 0)
                {
                    writeLog(getNowTime(), " accept client error, errno:", strerror(errno));
                    continue;
                }
                setNonblock(client_fd);
                // wait client sending type message of request
                epoll_event event;
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
                {
                    writeLog(getNowTime(), " epoll_ctr error, errno:", strerror(errno));
                    continue;
                }
            }
            // client sends request type
            else if (events[i].events & EPOLLIN)
            {
                memset(request_type, 0, request_type_max_size * sizeof(char));
                int client_fd = events[i].data.fd;
                // read request type
                int read_len = read(client_fd, request_type, request_type_max_size);
                if (read_len < 0)
                {
                    writeLog(getNowTime(), " read request error, errno:", strerror(errno));
                }
                // client has closed, it means client not sending any data
                else if (read_len == 0)
                {
                    writeLog(getNowTime(), " client(fd:", client_fd, ") closed before sending request type");
                }
                // request_type must contain '\0', but this position is occupied by request.
                else if (read_len == request_type_max_size)
                {
                    writeLog(getNowTime(), "client(fd:", client_fd, ") send invalid request type, it's too long");
                }
                // the rest of data will be read in the corresponding service.
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                if (read_len > 0 && read_len < request_type_max_size)
                    startService(client_fd, request_type);
            }
            else
            {
                cerr << "unknown event type" << endl;
            }
        }
    }
}

int main()
{
    WebFrame web_frame;

    web_frame.run();
}
