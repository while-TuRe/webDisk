#include <string>
#include <sys/time.h>
#include<fstream>
#include"const.h"
using namespace std;

string getNowTime();


void writeLog();
template <typename T, typename... Types>
void writeLog(const T& first_log, const Types&... rest_log);