#pragma once

struct AVFrame;
struct AVPacket;
//音视频编码接口类
class AVCodecContext;
class XMediaEncode
{
public:
	//输入参数
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;

	//输出参数
	int outWidth = 1280;
	int outHeight = 720;
	int bitRate = 50 * 1024 * 8;//比特率
	int outFps = 25;
public:
	//工厂的生产方法
	static XMediaEncode* GetInstance(unsigned char index = 0);//最多创建255个对象
	//初始化像素格式转换上下文
	virtual bool InitScale() = 0;
	//RGB to YUV
	virtual AVFrame* RGBToYUV(char* rgb) = 0;
	//视频编码器初始化
	virtual bool InitVideoCodec() = 0;
	//视频编码
	virtual AVPacket* EncodeVideo(AVFrame* frame) = 0;

	virtual ~XMediaEncode();
public:
	AVCodecContext* vc = 0;//视频编码器上下文
protected:
	XMediaEncode();//限制用户创建对象

};

