#pragma once
#include "XRtmp.h"
extern"C"
{
#include<libavformat/avformat.h>
}
#include <iostream>
#include <string>
#pragma comment(lib,"avformat.lib")
#include<iostream>
#include <string>
using namespace std;
class CRtmp :public XRtmp
{
public:
	//��ʼ����װ��
	bool Init(const char* url);
	//�ر�
	void close();
	//�����Ƶ��
	bool AddStream(const AVCodecContext* c);
	//���ͷ�װͷ
	bool SendHead();
	//����
	bool SendFrame(AVPacket* pack);
public:
	//rtmp flv ��װ��
	AVFormatContext* ic = NULL;
	string url;
	//��Ƶ��������
	const AVCodecContext* vc = NULL;
	AVStream* vs = NULL;
};

