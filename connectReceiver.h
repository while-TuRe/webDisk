#include <sys/epoll.h>
#include <functional>
#include <unordered_map>
#include <thread>

#include "FDMgr.h"
#include "const.h"
#include "util.h"

using namespace std;
using namespace std::this_thread;

class WebFrame
{
private:
    int epoll_fd;
    int accept_client_fd;
    unordered_map<string, thread::id> transaction_threads;

    void startService(int client_fd, string request_type);

public:
    WebFrame();

    ~WebFrame();

    void setNonblock(int fd);

    void route(string url, function<void()> func);

    void init();

    void run();
};
