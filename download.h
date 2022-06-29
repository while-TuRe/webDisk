#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <fstream>


struct FDInfo
{
    int fd;
    ifstream i_file;  // file to download
    int start_pos;
    int data_len;
};

class DownloadHelper
{
public:
    int epoll_fd;
    const char seperator;
    // whhen it decreases to 0, this service round ends.
    int fd_cnt;

    DownloadHelper();

    ~DownloadHelper();

    void closeDownloadFd(FDInfo *info);
};


void download();