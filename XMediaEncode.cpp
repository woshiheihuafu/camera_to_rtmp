#include "XMediaEncode.h"
#include "CMediaEncode.h"
XMediaEncode* XMediaEncode::GetInstance(unsigned char index)
{
	static bool isFirst = true;
	if (isFirst)
	{
		//ע�����еı������
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
