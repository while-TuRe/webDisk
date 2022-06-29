#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

#include "upload.h"
#include "FDMgr.h"
#include "config.h"
#include "util.h"
#include "configor/json.hpp"

using namespace std;
using namespace configor;

struct Client
{
    int fd;
    int start_pos;
    int end_pos;
};

struct Clip
{
    int start_pos;
    int end_pos;
};

struct UploadingFile
{
    string file_name;
    int has_uploaded_bytes_num;
    // is_uploading = [true, false]
    unordered_map<Client *, bool> clients;
    vector<Clip> to_be_uploaded_clips;
    ofstream o_file;
};

class UploadHelper
{
public:
    int epoll_fd;
    vector<UploadingFile> files;
    int fd_num;

    UploadHelper()
    {
        epoll_fd = epoll_create(epoll_size);
        if (epoll_fd < 0)
        {
            writeLog(getNowTime(), "create epoll error in download");
            exit(-1);
        }
        client_num = 0;
    }

    ~UploadHelper()
    {
        close(epoll_fd);
    }

    void closeFd(Client *client)
    {
    }

    // take out all fds in each round of service
    void acceptAllClients()
    {
        while (fd_mgr.getFdNum() > 0)
        {
            epoll_event event;
            event.events = EPOLLIN;
            Client *client = new (nothrow) Client;
            client->fd = fd_mgr.popFront();
            event.data.ptr = client;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client->fd, &event) < 0)
            {
                writeLog(getNowTime(), " epoll_ctr error, errno:", strerror(errno));
                closeFd(client);
                continue;
            }
            fd_num++;
        }
    }
};

void upload()
{
    UploadHelper helper;
    epoll_event events[epoll_size];

    helper.acceptAllClients();

    // read/write sharing
    char *buffer = new (nothrow) char[buffer_size];
    while (true)
    {
        // block service, so it won't serve fds in next round unitl
        // its service ends in this round.
        int event_num = epoll_wait(helper.epoll_fd, events, epoll_size, -1);
        for (int i = 0; i < event_num; i++)
        {
            memset(buffer, 0, buffer_size);
            Client *client = reinterpret_cast<Client *>(events[i].data.ptr);
            // get infomation about user
            if (events[i].events & EPOLLIN)
            {
                int client_fd = client->fd;
                epoll_ctl(helper.epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                int read_len = read(client_fd, buffer, buffer_size);
                // read error
                if (read_len < 0)
                {
                    writeLog(getNowTime(), " read request error, errno:", strerror(errno));
                    helper.closeFd(client);
                }
                // client has closed, it means client not sending any data
                else if (read_len == 0)
                {
                    writeLog(getNowTime(), " client(fd:", client_fd, ") closed before authorizing");
                    helper.closeFd(client);
                }
                // request_type must contain '\0', but this position is occupied by request.
                else if (read_len == buffer_size)
                {
                    writeLog(getNowTime(), "client(fd:", client_fd, ") send invalid file type, it's too long");
                    helper.closeFd(client);
                }
                // get it
                else
                {
                    // split user information from string
                    // (username, password)
                    cout << buffer << endl;
                    vector<string> params = split(string(buffer), ' ');
                    info->username = params[0];
                    info->password = params[1];

                    epoll_event event;
                    event.events = EPOLLOUT;
                    event.data.ptr = info;
                    epoll_ctl(authenticator.epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                }
            }
            // return result
            else
            {
                json j;
                j["ack"] = 1;
                j["cookie"] = "it's a cookie";
                string j_str = j.dump();
                write(info->fd, j_str.c_str(), j_str.length());
                writeLog(getNowTime(), " write to ", info->fd, "(fd) ", j_str.length(), " bytes.");
                authenticator.closeFd(info);
            }
        }
    }

    delete[] buffer;
}

void registerAccount()
{
    Authenticator authenticator;
    epoll_event events[epoll_size];

    // take out all fds in each round of service
    while (fd_mgr.getFdNum() > 0)
    {
        epoll_event event;
        event.events = EPOLLIN;
        UserInfo *info = new (nothrow) UserInfo;
        info->fd = fd_mgr.popFront();
        event.data.ptr = info;
        if (epoll_ctl(authenticator.epoll_fd, EPOLL_CTL_ADD, info->fd, &event) < 0)
        {
            writeLog(getNowTime(), " epoll_ctr error, errno:", strerror(errno));
            authenticator.closeFd(info);
            continue;
        }
        authenticator.fd_cnt++;
    }

    // read/write sharing
    char *buffer = new (nothrow) char[buffer_size];
    while (authenticator.fd_cnt > 0)
    {
        // block service, so it won't serve fds in next round unitl
        // its service ends in this round.
        int event_num = epoll_wait(authenticator.epoll_fd, events, epoll_size, -1);
        for (int i = 0; i < event_num; i++)
        {
            memset(buffer, 0, buffer_size);
            UserInfo *info = reinterpret_cast<UserInfo *>(events[i].data.ptr);
            // get infomation about user
            if (events[i].events & EPOLLIN)
            {
                int client_fd = info->fd;
                epoll_ctl(authenticator.epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                int read_len = read(client_fd, buffer, buffer_size);
                // read error
                if (read_len < 0)
                {
                    writeLog(getNowTime(), " read request error, errno:", strerror(errno));
                    authenticator.closeFd(info);
                }
                // client has closed, it means client not sending any data
                else if (read_len == 0)
                {
                    writeLog(getNowTime(), " client(fd:", client_fd, ") closed before authorizing");
                    authenticator.closeFd(info);
                }
                // request_type must contain '\0', but this position is occupied by request.
                else if (read_len == buffer_size)
                {
                    writeLog(getNowTime(), "client(fd:", client_fd, ") send invalid file type, it's too long");
                    authenticator.closeFd(info);
                }
                // get it
                else
                {
                    // split user information from string
                    // (username, password)
                    vector<string> params = split(string(buffer), ' ');
                    info->username = params[0];
                    info->password = params[1];

                    epoll_event event;
                    event.events = EPOLLOUT;
                    event.data.ptr = info;
                    epoll_ctl(authenticator.epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                }
            }
            // return result
            else
            {
                // writeDB(info->username, info->password);

                json j;
                j["ack"] = 1;
                string j_str = j.dump();
                write(info->fd, j_str.c_str(), j_str.length());
                writeLog(getNowTime(), " write to ", info->fd, "(fd) ", j_str.length(), " bytes.");
                authenticator.closeFd(info);
            }
        }
    }

    delete[] buffer;
}
