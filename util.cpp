#include<mutex>
#include<vector>
#include<sstream>
#include<string>

#include"config.h"
#include"util.h"

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


void writeLog() {
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

