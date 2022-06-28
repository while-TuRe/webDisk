#include<mutex>

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

template <typename T, typename... Types>
void writeLog(const T& first_log, const Types&... rest_log) {
	mtx.lock();
	log_file << first_log;
	writeLog(rest_log...);
	mtx.unlock();
}

