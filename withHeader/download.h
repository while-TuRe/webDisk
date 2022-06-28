#pragma once

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

#include "webFrame.h"
#include "config.h"
#include "util.h"

struct FDInfo
{
    int fd;
    ifstream i_file;
    int start_pos;
    int data_len;
};

class DownloadHelper
{
public:
    int epoll_fd;
    const char seperator;
    int fd_cnt;

    DownloadHelper();

    ~DownloadHelper();

    void closeDownloadFd(FDInfo *info);
};

vector<string> split(const string &s, const char seperator);

void download();