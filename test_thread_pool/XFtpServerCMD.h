#pragma once

#include "XTask.h"
class XFtpServerCMD :public XTask
{
public:

	//��ʼ������
	bool Init() override;

	XFtpServerCMD();
	~XFtpServerCMD();
};