#include <sys/epoll.h>
#include <functional>
#include <unordered_map>
#include <thread>

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
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>

#include"download.h"
#include "config.h"
#include "util.h"

/*
struct FDInfo
{
    int fd;
    ifstream i_file;
    int start_pos;
    int data_len;
};*/


    DownloadHelper::DownloadHelper() : seperator(' ')
    {
        epoll_fd = epoll_create(epoll_size);
        if (epoll_fd < 0)
        {
            writeLog(getNowTime(), "create epoll error in download");
            return;
        }
        fd_cnt = 0;
    }

    DownloadHelper::~DownloadHelper()
    {
        close(epoll_fd);
    }

    void DownloadHelper::closeDownloadFd(FDInfo *info)
    {
        fd_cnt--;
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, info->fd, nullptr);
        close(info->fd);
        info->i_file.close();
        delete info;
    }


vector<string> split(const string &s, const char seperator)
{

    stringstream ss(s);
    string item;
    vector<string> v;

    while (getline(ss, item, seperator))
    {
        v.push_back(item);
    }
    return v;
}

void download()
{
    DownloadHelper download_helper;
    FDMgr &fd_mgr = web_frame.fd_mgr;
    epoll_event events[epoll_size];

    while (fd_mgr.getFdNum() > 0)
    {
        epoll_event event;
        event.events = EPOLLIN;
        FDInfo *info = new (nothrow) FDInfo;
        info->fd = fd_mgr.popFront();
        event.data.ptr = info;
        if (epoll_ctl(download_helper.epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) < 0)
        {
            writeLog(getNowTime(), " epoll_ctr error, errno:", strerror(errno));
            download_helper.closeDownloadFd(info);
            continue;
        }
        download_helper.fd_cnt++;
    }

    while (download_helper.fd_cnt)
    {
        int event_num = epoll_wait(download_helper.epoll_fd, events, epoll_size, -1);
        char buffer[buffer_size];
        for (int i = 0; i < event_num; i++)
        {
            memset(buffer, 0, buffer_size);
            FDInfo *info = reinterpret_cast<FDInfo *>(events[i].data.ptr);

            if (events[i].events & EPOLLIN)
            {
                int client_fd = info->fd;
                epoll_ctl(download_helper.epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);

                int read_len = read(client_fd, buffer, buffer_size);
                if (read_len < 0)
                {
                    writeLog(getNowTime(), " read request error, errno:", strerror(errno));
                    download_helper.closeDownloadFd(info);
                }
                // client has closed, it means client not sending any data
                else if (read_len == 0)
                {
                    writeLog(getNowTime(), " client(fd:", client_fd, ") closed before downloading");
                    download_helper.closeDownloadFd(info);
                }
                // request_type must contain '\0', but this position is occupied by request.
                else if (read_len == buffer_size)
                {
                    writeLog(getNowTime(), "client(fd:", client_fd, ") send invalid file type, it's too long");
                    download_helper.closeDownloadFd(info);
                }
                else
                {
                    vector<string> params = split(string(buffer), ' ');
                    info->i_file.open(params[0], ios::binary);
                    if (info->i_file.fail())
                    {
                        writeLog(getNowTime(), " file that client(fd:", client_fd, ") downloads can't open");
                        download_helper.closeDownloadFd(info);
                    }
                    info->start_pos = atoi(params[1].c_str());
                    info->data_len = atoi(params[2].c_str());
                    info->i_file.seekg(info->start_pos, ios::beg);
                    epoll_event event;
                    event.events = EPOLLOUT;
                    event.data.ptr = info;
                    epoll_ctl(download_helper.epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                }
            }
            else
            {
                int read_len = min(buffer_size, info->data_len);
                info->i_file.read(buffer, read_len);
                int write_len = write(info->fd, buffer, read_len);
                // I don't know what's the case when write_len < 0 !!!
                if (write_len <= 0)
                {
                    // buffer is full
                    if (errno == EAGAIN)
                    {
                        info->i_file.seekg(-read_len, ios::cur);
                    }
                    // server has closed
                    else if (errno == EPIPE || errno == ENOENT)
                    {
                        download_helper.closeDownloadFd(info);
                    }
                    else
                    {
                        writeLog(getNowTime(), " send to client, errno has an unknown errno:", strerror(errno));
                    }
                    if (write_len == -1)
                    {
                        writeLog(getNowTime(), "send to client write_len is -1, errno:", strerror(errno));
                    }
                }
                else
                {
                    info->data_len -= write_len;
                    if (info->data_len == 0)
                    {
                        download_helper.closeDownloadFd(info);
                    }
                }
            }
        }
    }
}

