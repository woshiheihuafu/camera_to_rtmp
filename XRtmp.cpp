#include "XRtmp.h"
#include "CRtmp.h"
XRtmp* XRtmp::GetInstance(unsigned char index)
{
	static bool isFirst = true;
	if (isFirst)
	{
		//注册所有的封装器件
		av_register_all();
		//注册所有网络协议
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
