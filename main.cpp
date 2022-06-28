#include"webFrame.h"
#include"download.h"

int main()
{
    web_frame.init();
    web_frame.route("download", download);
    web_frame.run();
}
