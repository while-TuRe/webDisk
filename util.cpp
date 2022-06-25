#include<mutex>

#include"util.h"

recursive_mutex mtx;

ofstream log(log_path, ios::app);

string getNowTime()
{
    timeval tv;
    char buf[64];
    gettimeofday(&tv, NULL);
    strftime(buf, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
    return string(buf);
}


void writeLog() {
	log << endl;
}

template <typename T, typename... Types>
void writeLog(const T& first_log, const Types&... rest_log) {
	mtx.lock();
	log << first_log;
	writeLog(rest_log...);
	mtx.unlock();
}
