#include "XRtmp.h"
#include "CRtmp.h"
XRtmp* XRtmp::GetInstance(unsigned char index)
{
	static bool isFirst = true;
	if (isFirst)
	{
		//ע�����еķ�װ����
		av_register_all();
		//ע����������Э��
		avformat_network_init();
		isFirst = false;
	}
	static CRtmp cxr[255];
	return &cxr[index];
}

XRtmp::XRtmp()
{
}

XRtmp::~XRtmp()
{
}
