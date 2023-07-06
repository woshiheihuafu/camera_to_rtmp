#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <Windows.h>
extern"C"
{
#include<libswscale/swscale.h>
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
}
#include <iostream>
#include <string>
using namespace std;
using namespace cv;
#pragma comment(lib,"opencv_world320d.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#include "XMediaEncode.h"
class CMediaEncode :public XMediaEncode
{
public:
	//初始化像素格式转换上下文
	bool InitScale();
	AVFrame* RGBToYUV(char* rgb);
	//视频编码器初始化
	bool InitVideoCodec();
	//视频编码
	AVPacket* EncodeVideo(AVFrame* frame);
	//关闭
	void close();
public:
private:
	SwsContext* vsc = NULL;//像素格式转换上下文
	AVFrame* yuv = NULL;//
	AVPacket pack = { 0 };
	int vpts = 0;
};

