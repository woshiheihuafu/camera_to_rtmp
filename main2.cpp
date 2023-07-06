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
	//���������rtsp url
	const char* inUrl = "rtsp://test:test123456@192.168.1.64";
	//nginx-rtmp ֱ��������rtmp����URL
	const char* outUrl = "rtmp://192.168.150.128/live/123456";

	//�����������ظ�ʽת��
	XMediaEncode* me = XMediaEncode::GetInstance(0);
	//��װ����������
	XRtmp* rm = XRtmp::GetInstance(0);
	//ע�����еķ�װ��
	av_register_all();

	//ע����������Э��
	avformat_network_init();


	VideoCapture cam;
	Mat frame;
	namedWindow("video");

	//rtmp flv ��װ��
	AVFormatContext* ic = NULL;


	try
	{	////////////////////////////////////////////////////////////////
		/// 1 ʹ��opencv��rtsp���
		cam.open(0);
		if (!cam.isOpened())
		{
			throw exception("cam open failed!");
		}
		cout << inUrl << " cam open success" << endl;
		cam.set(CV_CAP_PROP_FPS, 30);//֡�� ֡/��
		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = cam.get(CV_CAP_PROP_FPS);

		///2 ��ʼ����ʽת��������
		///3 ��ʼ����������ݽṹ
		me->inWidth = inWidth;
		me->inHeight = inHeight;
		me->outWidth = inWidth;
		me->outHeight = inHeight;
		me->InitScale();

		///4 ��ʼ������������
		if (!me->InitVideoCodec())
		{
			throw exception("InitVideoCodec error!");
		}

		///5 �����װ������Ƶ������
		if (!rm->Init(outUrl))
		{
			throw exception("Init avformat_alloc_output_context2 error!");
		}
		///6 �����Ƶ�� 
		if (!rm->AddStream(me->vc))
		{
			throw exception("AddStream error!");
		}
		///��rtmp ���������IO
		//д���װͷ
		if (!rm->SendHead())
		{
			throw exception("SendHead error!");
		}
		

		for (;;)
		{
			///��ȡrtsp��Ƶ֡��������Ƶ֡
			cam.read(frame);
			/*if (!cam.grab())
			{
				continue;
			}
			///yuvת��Ϊrgb
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
			///h264����
			AVPacket* pack = me->EncodeVideo(yuv);
			if (!pack) continue;
			//����
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