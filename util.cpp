#include <mutex>
#include <vector>
#include <sstream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tuple>
#include "config.h"
#include "util.h"

recursive_mutex mtx;

ofstream log_file(log_path, ios::app);

string getNowTime()
{
    timeval tv;
    char buf[64];
    gettimeofday(&tv, NULL);
    strftime(buf, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
    return string(buf);
}

void writeLog()
{
    log_file << endl;
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

tuple<string, int> getPeerIPWithPort(int fd)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    tuple<string, int> info;
    if (getpeername(fd, (struct sockaddr *)&addr, &len) != -1)
    {
        info = make_tuple(string(inet_ntoa(addr.sin_addr)), ntohs(addr.sin_port));
    }
    return info;
}