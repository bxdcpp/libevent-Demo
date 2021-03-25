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

void socket_readcb(struct bufferevent* bev, void* ctx)
{
    char msg[1024] = {0};
    size_t len = bufferevent_read(bev, msg, sizeof(msg) - 1);
    std::cout << "server read the data" << msg << std::endl;
    char reply[] = "I has read your data";
    bufferevent_write(bev, reply, strlen(reply));
}

void socket_writecb(struct bufferevent* bev, void* ctx)
{
    char reply[] = "I am a server";
    bufferevent_write(bev, reply, strlen(reply));
}


void socket_eventcb(bufferevent* bev, short events, void* arg)
{
    if (events & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (events & BEV_EVENT_ERROR)
        printf("some other error\n");

    //这将自动close套接字和free读写缓冲区  
    bufferevent_free(bev);

}

void listen_cb(struct evconnlistener* e, evutil_socket_t s, struct sockaddr* a, int socklen, void* arg)
{
    cout << "listen_cb" << endl;

    event_base* base = (event_base*)arg;

    //为这个客服端分配一个bufferevent
    bufferevent* bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, socket_readcb, socket_writecb, socket_eventcb, base);
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
    
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
    //监听端口
    //socket ，bind，listen 绑定事件
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);

    evconnlistener* ev = evconnlistener_new_bind(base,  // libevent的上下文
        listen_cb,                  //接收到连接的回调函数
        base,                       //回调函数获取的参数 arg
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,//地址重用，evconnlistener关闭同时关闭socket
        10,                         //连接队列大小，对应listen函数
        (sockaddr*)&sin,                            //绑定的地址和端口
        sizeof(sin)
    );
    //事件分发处理
    if (base)
        event_base_dispatch(base);
    if (ev)
        evconnlistener_free(ev);
    if (base)
        event_base_free(base);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}