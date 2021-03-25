#include <event2/event.h>
#include <event2/listener.h>
#include <event2\bufferevent.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <iostream>


using namespace std;
#define SPORT 5001

static int sendCount = 0;

void client_writecb(struct bufferevent* bev, void* ctx)
{
    /* char reply[] = "I am a client\n";
     bufferevent_write(bev, reply, strlen(reply));*/

    cout << "[client W]" << flush;
    FILE* fp = (FILE*)ctx;
    char data[1024] = { 0 };

    int len = fread(data, 1, sizeof(data) - 1, fp);
    if (len <= 0)
    {
        //读到结尾或在文件出错
        fclose(fp);
        //立刻清理可能会造成缓冲数据没有发送结束
        //	bufferevent_free(be);
        bufferevent_disable(bev, EV_WRITE);
        return;
    }
    sendCount += len;
    //写入buffer
    bufferevent_write(bev, data, len);
}

void client_readcb(struct bufferevent* bev, void* ctx)
{
    cout << "[client R]" << flush;
}


void client_eventcb(bufferevent* be, short events, void* arg)
{

    cout << "[client E]" << flush;
    //超时事件发生时，读取事件停止
    if (events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
    {
        cout << "BEV_EVENT_TIMEOUT BEV_EVENT_READING" << endl;
        //	bufferevent_enable(be, EV_READ);   //再次开启读事件
        bufferevent_free(be);			//超时清理掉链接
        return;
    }
    else if (events & BEV_EVENT_ERROR) {
        cout << "BEV_EVENT_ERROR" << endl;
        bufferevent_free(be);
        return;
    }

    //服务端的关闭事件
    if (events & BEV_EVENT_EOF)
    {
        cout << "BEV_EVENT_EOF" << endl;
        bufferevent_free(be);
    }

    if (events & BEV_EVENT_CONNECTED)
    {
        cout << "BEV_EVENT_CONNECTED" << endl;
        bufferevent_trigger(be, EV_WRITE, 0);
    }
}


int main()
{
    cout << "hello world!" << std::endl;

#ifdef _WIN32 
    //初始化socket库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    //忽略管道信号，发送数据给已关闭的socket
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return 1;
#endif

    std::cout << "test server!\n";
    //创建libevent的上下文
    event_base* base = event_base_new();
    if (base)
    {
        cout << "event_base_new success!" << endl;
    }


    struct bufferevent* bev = bufferevent_socket_new(base, -1,
        BEV_OPT_CLOSE_ON_FREE);



    ////监听终端输入事件
    //struct event* ev_cmd = event_new(base,  ,
    //    EV_READ | EV_PERSIST, cmd_msg_cb, (void*)bev);

    FILE* fp = fopen("./test.txt", "rb");
    if (fp == nullptr)
    {
        std::cerr << "fopen failed!" << std::endl;
    }

    bufferevent_setcb(bev, client_readcb, client_writecb,client_eventcb, fp);
    //持久事件
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);


    //socket 绑定事件
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);
    //inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
    evutil_inet_pton(AF_INET, "192.168.0.181", &sin.sin_addr);

    bufferevent_socket_connect(bev, (struct sockaddr*)&sin,
        sizeof(sin));

    //事件分发处理
    if (base)
        event_base_dispatch(base);

    if (base)
        event_base_free(base);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}