#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"

#include <thread>
#include <iostream>

//***********************************
//XThreadPool�����̺߳ͷַ�����
//***********************************


using namespace std;
//init
XThreadPool* XThreadPool::m_pInstance = nullptr;

void XThreadPool::Init(int threadCount)
{
	this->threadCount = threadCount;
	for (int i = 0; i < threadCount; i++)
	{

		XThread* t = new XThread();
		cout << "Create thread" << i << endl;
		t->SetID(i+1);
		t->Start();
		threads.push_back(t);
		std::this_thread::sleep_for(10ms);
	}

}

void XThreadPool::Dispatch(XTask* task)
{
	//��ѯ
	if (!task) return;
	int tid = (lastThread + 1) % threadCount;
	lastThread = tid;
	XThread* t = threads[tid];

	t->AddTask(task);

	//�����߳�
	t->Active();
}

XThreadPool::XThreadPool() = default;

XThreadPool::~XThreadPool()
{
}
