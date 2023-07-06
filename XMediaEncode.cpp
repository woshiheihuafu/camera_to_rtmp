#include "XMediaEncode.h"
#include "CMediaEncode.h"
XMediaEncode* XMediaEncode::GetInstance(unsigned char index)
{
	static bool isFirst = true;
	if (isFirst)
	{
		//注册所有的编解码器
		avcodec_register_all();
		isFirst = false;
	}
	static CMediaEncode cxm[255];
	return &cxm[index];
}

XMediaEncode::XMediaEncode()
{
}

XMediaEncode::~XMediaEncode()
{
}
