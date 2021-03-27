#include "XFtpServerCMD.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <iostream>
//#ifdef _WIN32
//#else
//#include <signal.h>
//#endif // _WIN32

using namespace std;


void EventCB(struct bufferevent* bev, short what, void* arg)
{
	XFtpServerCMD* cmd = (XFtpServerCMD*)arg;
	//����Է�����ϵ������ڻ���˾�������ղ���EOF����ERR
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(bev);
		delete cmd;
	}
}

//���߳�XThread event�¼��ַ�
static void ReadCB(bufferevent* bev, void* arg)
{
	char data[1024] = { 0 };
	for (;;)
	{
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0) return;
		data[len] = '\0';
		cout << data << flush;
	}
}

//��ʼ���������������߳���
bool XFtpServerCMD::Init()
{
	cout << "XFtpServerCMD::Init()" << endl;
	//����socket
	bufferevent* bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, ReadCB, 0, EventCB, this);
	bufferevent_enable(bev, EV_READ | EV_WRITE);


	//��ӳ�ʱ
	timeval rt = { 10, 0 };
	bufferevent_set_timeouts(bev, &rt, 0);
	return true;
}

XFtpServerCMD::XFtpServerCMD()
{

}

XFtpServerCMD::~XFtpServerCMD()
{

}
