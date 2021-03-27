#include "XThread.h"
#include "XTask.h"

#include <event2/event.h>
#include <event2/bufferevent.h>

#include <thread>
#include <iostream>

#ifdef _WIN32
#else
#include <unistd.h>
#endif // _WIN32

using namespace std;

static void NotifyCB(evutil_socket_t fd, short which, void* arg)
{
	XThread* t = (XThread*)arg;
	t->Notify(fd, which);
}

XThread::XThread() = default;

XThread::~XThread()
{
}

void XThread::Start()
{
	//线程设置
	Setup();
	//启动线程
	thread th(&XThread::Main, this);
	//断开与主线程的联系
	th.detach();
}

void XThread::Main()
{
	std::cout << m_id <<"["<< __func__ <<"]"<< "begin" << std::endl;
	event_base_dispatch(base);
	event_base_free(base);
	std::cout << m_id << "[" << __func__ << "]"<< "end" << std::endl;

}

bool XThread::Setup()
{
	//windows用配对socket linux用管道
#ifdef _WIN32
	//创建一个soketpair 可以互相通信 fds[0] 读 fds[1] 写
	evutil_socket_t fds[2];
	if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
	{
		cout << "evutil_socketpair failed!" << std::endl;
	}
	//设置为非阻塞
	evutil_make_socket_nonblocking(fds[0]);
	evutil_make_socket_nonblocking(fds[1]);
#else
	int fds[2];
	if (pipe(fds))
	{
		cerr << "pip failed!" << endl;
		return false;
	}

#endif //_WIN32

	//读取绑定到event事件中，写入要保存
	notify_send_fd = fds[1];


	//创建libevent上下文(无锁)
	event_config* ev_conf = event_config_new();
	event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
	this->base = event_base_new_with_config(ev_conf);
	event_config_free(ev_conf);
	if (!base)
	{
		cerr << "event_base_new_with_config failed in thread!" << endl;
		return false;
	}

	//添加管道监听事件，用于激活线程执行任务
	event* ev = event_new(base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
	event_add(ev, 0);




	return true;
}

void XThread::Notify(evutil_socket_t fd, short which)
{
	//水平触发，只要没有接收完全，会再次进来
	char buf[2] = { 0 };
#ifdef _WIN32
	int re = recv(fd, buf, 1, 0);
#else
	//linux 是管道不能用recv
	int re = read(fd, buf, 1);
#endif

	if (re <= 0)
		return;
	cout << "id = " << m_id << " thread " << buf << endl;
	XTask* task = NULL;
	//获取任务，并初始化任务

	tasks_mutex.lock();
	if (tasks.empty())
	{
		tasks_mutex.unlock();
		return;
	}
	task = tasks.front(); 	//先进先出
	tasks.pop_front();
	tasks_mutex.unlock();
	task->Init();
}

void XThread::Active()
{
#ifdef _WIN32
	int ret = send(this->notify_send_fd, "C", 1, 0);
#else
	int ret = write(this->notify_send_fd, "C", 1);
#endif // _WIN32
	if (ret <= 0)
	{
		cerr << __FILE__ << __FUNCTION__ << "failed!" << endl;
	}

}

void XThread::AddTask(XTask* t)
{
	if (!t) return;
	t->base = this->base;
	tasks_mutex.lock();
	tasks.push_back(t);
	tasks_mutex.unlock();

}

void XThread::SetID(int id)
{
	m_id = id;
}

