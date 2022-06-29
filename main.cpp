#include"webFrame.h"
#include"download.h"
// #include"authorize.h"

int main()
{
    web_frame.init();
    web_frame.route("download", download);
    // web_frame.route("login", login);
    // web_frame.route("register", registerAccount);
    
    web_frame.run();
}
