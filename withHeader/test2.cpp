#include<iostream>
#include<thread>
#include<unistd.h>

using namespace std;
using namespace std::this_thread;

// int main() {
//     thread t([]{cout << "thread:" << get_id() << endl;});
//     t.join();
// }