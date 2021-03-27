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
	//�߳�����
	Setup();
	//�����߳�
	thread th(&XThread::Main, this);
	//�Ͽ������̵߳���ϵ
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
	//windows�����socket linux�ùܵ�
#ifdef _WIN32
	//����һ��soketpair ���Ի���ͨ�� fds[0] �� fds[1] д
	evutil_socket_t fds[2];
	if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
	{
		cout << "evutil_socketpair failed!" << std::endl;
	}
	//����Ϊ������
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

	//��ȡ�󶨵�event�¼��У�д��Ҫ����
	notify_send_fd = fds[1];


	//����libevent������(����)
	event_config* ev_conf = event_config_new();
	event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
	this->base = event_base_new_with_config(ev_conf);
	event_config_free(ev_conf);
	if (!base)
	{
		cerr << "event_base_new_with_config failed in thread!" << endl;
		return false;
	}

	//��ӹܵ������¼������ڼ����߳�ִ������
	event* ev = event_new(base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
	event_add(ev, 0);




	return true;
}

void XThread::Notify(evutil_socket_t fd, short which)
{
	//ˮƽ������ֻҪû�н�����ȫ�����ٴν���
	char buf[2] = { 0 };
#ifdef _WIN32
	int re = recv(fd, buf, 1, 0);
#else
	//linux �ǹܵ�������recv
	int re = read(fd, buf, 1);
#endif

	if (re <= 0)
		return;
	cout << "id = " << m_id << " thread " << buf << endl;
	XTask* task = NULL;
	//��ȡ���񣬲���ʼ������

	tasks_mutex.lock();
	if (tasks.empty())
	{
		tasks_mutex.unlock();
		return;
	}
	task = tasks.front(); 	//�Ƚ��ȳ�
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

