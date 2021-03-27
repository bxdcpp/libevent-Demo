#pragma once
#include <event2\util.h>
#include <list>
#include <mutex>

////socket event_base---> thread event_base----> task event
//***********************************
//每个XThread都有一个自己的libevent 上下文
//每个任务都有自己的bufferevent 
//这个buffer根据绑定的线程的event_base创建
//通过管道或者socketpair通信来激活任务
//***********************************

class XTask;
class XThread
{
public:
	XThread();
	~XThread();

	//启动线程
	void Start();
	//线程入口函数
	void Main();

	//安装线程，初始化event_base 和管道监听时间用于激活
	bool Setup();

	//收到主线程发出的激活消息(线程池的分发)
	void Notify(evutil_socket_t fd, short which);

	void Active();

	//添加处理的任务, 一个线程可以处理多个任务，共用一个event_base
	void AddTask(XTask* t);

	void SetID(int id);
private:

	int notify_send_fd = 0;
	struct event_base* base = 0;
	//任务列表
	std::list<XTask*> tasks;
	//线程安全
	std::mutex tasks_mutex;
	//线程编号
	int m_id = 0;
};

