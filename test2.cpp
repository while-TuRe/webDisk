#include<iostream>
#include<thread>
#include<unistd.h>

using namespace std;
using namespace std::this_thread;


void f(thread::id tid) {
    cout << tid << endl;
}

// int main() {
//     thread::id tid = get_id();
//     f(tid);
// }
