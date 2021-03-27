#pragma once
#include <event2\util.h>
#include <list>
#include <mutex>

////socket event_base---> thread event_base----> task event
//***********************************
//ÿ��XThread����һ���Լ���libevent ������
//ÿ���������Լ���bufferevent 
//���buffer���ݰ󶨵��̵߳�event_base����
//ͨ���ܵ�����socketpairͨ������������
//***********************************

class XTask;
class XThread
{
public:
	XThread();
	~XThread();

	//�����߳�
	void Start();
	//�߳���ں���
	void Main();

	//��װ�̣߳���ʼ��event_base �͹ܵ�����ʱ�����ڼ���
	bool Setup();

	//�յ����̷߳����ļ�����Ϣ(�̳߳صķַ�)
	void Notify(evutil_socket_t fd, short which);

	void Active();

	//��Ӵ��������, һ���߳̿��Դ��������񣬹���һ��event_base
	void AddTask(XTask* t);

	void SetID(int id);
private:

	int notify_send_fd = 0;
	struct event_base* base = 0;
	//�����б�
	std::list<XTask*> tasks;
	//�̰߳�ȫ
	std::mutex tasks_mutex;
	//�̱߳��
	int m_id = 0;
};

