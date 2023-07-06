#include "CMediaEncode.h"

#if defined WIN32 || defined _WIN32
#include<Windows.h>
#endif

//��ȡCPU����
static int GetCpuNum()
{
#if defined WIN32 || defined _WIN32
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return (int)sysInfo.dwNumberOfProcessors;
#elif defined __linux__
	return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined __APPLE__
	int numCPU = 0;
	int mib[4];
	size_t len = sizeof(numCPU);

	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;

	sysctl(mib, 2, &numCPU, &len, NULL, 0);
	if (numCPU < 1)
	{
		mib[1] = HW_NCPU;
		sysctl(mib, 2, &numCPU, &len, NULL, 0);
		if (numCPU < 1)
		{
			numCPU = 1;
		}
	}
	return (int)numCPU;
#else
	return 1;
#endif
}
//��ʼ�����ظ�ʽת��������
bool CMediaEncode::InitScale()
{
	///   ��ʼ�����ظ�ʽת��������
	vsc = sws_getCachedContext(vsc,
		inWidth, inHeight, AV_PIX_FMT_BGR24,	 //Դ���ߡ����ظ�ʽ
		outWidth,outHeight, AV_PIX_FMT_YUV420P,//Ŀ����ߡ����ظ�ʽ
		SWS_BICUBIC,  // �ߴ�仯ʹ���㷨
		0, 0, 0
	);
	if (!vsc)
	{
		cout << "sws_getCachedContext failed!" << endl;
		return false;
	}
	///   ��ʼ����������ݽṹ
	yuv = av_frame_alloc();
	yuv->format = AV_PIX_FMT_YUV420P;
	yuv->width = inWidth;
	yuv->height = inHeight;
	yuv->pts = 0;
	//   ����yuv�ռ�
	int ret = av_frame_get_buffer(yuv, 32);
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}

AVFrame* CMediaEncode::RGBToYUV(char* rgb)
{
	///rgb to yuv
			//��������ݽṹ
	uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
	//indata[0] bgrbgrbgr
	//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
	indata[0] = (uint8_t*)rgb;
	int insize[AV_NUM_DATA_POINTERS] = { 0 };
	//һ�У������ݵ��ֽ���
	insize[0] = inWidth * inPixSize;
	int h = sws_scale(vsc, indata, insize, 0, inHeight, //Դ����
		yuv->data, yuv->linesize);
	if (h <= 0)
	{
		return NULL;
	}
	return yuv;
}
//��Ƶ��������ʼ��
bool CMediaEncode::InitVideoCodec()
{
	///4 ��ʼ������������
		//a �ҵ�������
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec)
	{
		cout << "Can`t find h264 encoder!" << endl;
		return false;
	}
	//b ����������������
	vc = avcodec_alloc_context3(codec);
	if (!vc)
	{
		cout << "avcodec_alloc_context3 failed!" << endl;
		return false;
	}
	//c ���ñ���������
	vc->flags = AV_CODEC_FLAG_GLOBAL_HEADER; //ȫ�ֲ���
	vc->codec_id = codec->id;
	vc->thread_count = GetCpuNum();

	vc->bit_rate = 50 * 1024 * 8;//ѹ����ÿ����Ƶ��bitλ��С 50kB
	vc->width = outWidth;
	vc->height = outHeight;
	vc->time_base = { 1,outFps };
	vc->framerate = { outFps,1 };

	//������Ĵ�С������֡һ���ؼ�֡
	vc->gop_size = 50;
	vc->max_b_frames = 0;
	vc->pix_fmt = AV_PIX_FMT_YUV420P;
	//d �򿪱�����������
	int re = avcodec_open2(vc, 0, 0);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	cout << "avcodec_open2 success!" << endl;
	return true;
}
//��Ƶ����
AVPacket* CMediaEncode::EncodeVideo(AVFrame* frame)
{
	av_packet_unref(&pack);
	///h264����
	frame->pts = vpts;
	vpts++;
	int re = avcodec_send_frame(vc, frame);
	if (re != 0)
		return NULL;

	re = avcodec_receive_packet(vc, &pack);
	if(re != 0 || pack.size <= 0)
		return NULL;
	return &pack;
}

void CMediaEncode::close()
{
	if (vsc)
	{
		sws_freeContext(vsc);
		vsc = NULL;
	}
	if (yuv)
	{
		av_frame_free(&yuv);//�Զ���NULL
	}
}
