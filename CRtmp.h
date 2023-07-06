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
	//初始化封装器
	bool Init(const char* url);
	//关闭
	void close();
	//添加视频流
	bool AddStream(const AVCodecContext* c);
	//发送封装头
	bool SendHead();
	//推流
	bool SendFrame(AVPacket* pack);
public:
	//rtmp flv 封装器
	AVFormatContext* ic = NULL;
	string url;
	//视频编码器流
	const AVCodecContext* vc = NULL;
	AVStream* vs = NULL;
};

