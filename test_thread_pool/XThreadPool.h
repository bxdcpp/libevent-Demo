#pragma once
#include <vector>

////socket event_base---> thread event_base----> task event



class XTask;
class XThread;
class XThreadPool
{
public:
	inline static  XThreadPool* Instance(){

		if (m_pInstance == nullptr)
			m_pInstance = new XThreadPool();

		return m_pInstance;
	}

	void Init(int threadCount);

	//分发线程
	void Dispatch(XTask* task);

private:

	XThreadPool();
	~XThreadPool();
	//线程数量
	int threadCount = 0;
	int lastThread = -1;
	//线程池线程
	std::vector<XThread*> threads;

	static XThreadPool* m_pInstance;
};

