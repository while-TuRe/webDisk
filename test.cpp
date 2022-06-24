#include <iostream>
#include <vector>
#include <string>
#include<thread>
#include<unordered_map>
#include<functional>
using namespace std;

class WebFrame
{
private:
    unordered_map<string, thread> transaction_threads;
    unordered_map<string, function<void()>> url_map;

public:


    void route(string url, function<void()> func)
    {
        if (url_map.find(url) != url_map.end())
        {
            cerr << "url repeated" << endl;
            return;
        }

        auto wrapper = [&]
        {
            cout << "something before calling" << endl;
            func();
            cout << "something after calling" << endl;
        };

        url_map[url] = wrapper;
    }

    void run(string url)
    {
        url_map[url]();
    }
};

void f() {
    cout << "f" << endl;
}

int main()
{
    WebFrame web_frame;
    web_frame.route("test", f);
    web_frame.run("test");
}