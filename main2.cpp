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
#include "XRtmp.h"
int main(int argc, char* argv[])
{
	//海康相机的rtsp url
	const char* inUrl = "rtsp://test:test123456@192.168.1.64";
	//nginx-rtmp 直播服务器rtmp推流URL
	const char* outUrl = "rtmp://192.168.150.128/live/123456";

	//编码器和像素格式转换
	XMediaEncode* me = XMediaEncode::GetInstance(0);
	//封装和推流对象
	XRtmp* rm = XRtmp::GetInstance(0);
	//注册所有的封装器
	av_register_all();

	//注册所有网络协议
	avformat_network_init();


	VideoCapture cam;
	Mat frame;
	namedWindow("video");

	//rtmp flv 封装器
	AVFormatContext* ic = NULL;


	try
	{	////////////////////////////////////////////////////////////////
		/// 1 使用opencv打开rtsp相机
		cam.open(0);
		if (!cam.isOpened())
		{
			throw exception("cam open failed!");
		}
		cout << inUrl << " cam open success" << endl;
		cam.set(CV_CAP_PROP_FPS, 30);//帧率 帧/秒
		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = cam.get(CV_CAP_PROP_FPS);

		///2 初始化格式转换上下文
		///3 初始化输出的数据结构
		me->inWidth = inWidth;
		me->inHeight = inHeight;
		me->outWidth = inWidth;
		me->outHeight = inHeight;
		me->InitScale();

		///4 初始化编码上下文
		if (!me->InitVideoCodec())
		{
			throw exception("InitVideoCodec error!");
		}

		///5 输出封装器和视频流配置
		if (!rm->Init(outUrl))
		{
			throw exception("Init avformat_alloc_output_context2 error!");
		}
		///6 添加视频流 
		if (!rm->AddStream(me->vc))
		{
			throw exception("AddStream error!");
		}
		///打开rtmp 的网络输出IO
		//写入封装头
		if (!rm->SendHead())
		{
			throw exception("SendHead error!");
		}
		

		for (;;)
		{
			///读取rtsp视频帧，解码视频帧
			cam.read(frame);
			/*if (!cam.grab())
			{
				continue;
			}
			///yuv转换为rgb
			if (!cam.retrieve(frame))
			{
				continue;
			}*/
			//imshow("video", frame);
			//waitKey(1);


			///rgb to yuv
			me->inPixSize = frame.elemSize();
			AVFrame* yuv = me->RGBToYUV((char*)frame.data);
			if (!yuv) continue;
			///h264编码
			AVPacket* pack = me->EncodeVideo(yuv);
			if (!pack) continue;
			//推流
			rm->SendFrame(pack);
		}

	}
	catch (exception& ex)
	{
		if (cam.isOpened())
			cam.release();
		//if (vsc)
		//{
		//	sws_freeContext(vsc);
		//	vsc = NULL;
		//}

		if (me->vc)
		{
			avio_closep(&ic->pb);
			avcodec_free_context(&me->vc);
		}

		cerr << ex.what() << endl;
	}
	getchar();
	return 0;
}