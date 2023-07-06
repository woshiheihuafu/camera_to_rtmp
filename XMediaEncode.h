#pragma once

struct AVFrame;
struct AVPacket;
//����Ƶ����ӿ���
class AVCodecContext;
class XMediaEncode
{
public:
	//�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;

	//�������
	int outWidth = 1280;
	int outHeight = 720;
	int bitRate = 50 * 1024 * 8;//������
	int outFps = 25;
public:
	//��������������
	static XMediaEncode* GetInstance(unsigned char index = 0);//��ഴ��255������
	//��ʼ�����ظ�ʽת��������
	virtual bool InitScale() = 0;
	//RGB to YUV
	virtual AVFrame* RGBToYUV(char* rgb) = 0;
	//��Ƶ��������ʼ��
	virtual bool InitVideoCodec() = 0;
	//��Ƶ����
	virtual AVPacket* EncodeVideo(AVFrame* frame) = 0;

	virtual ~XMediaEncode();
public:
	AVCodecContext* vc = 0;//��Ƶ������������
protected:
	XMediaEncode();//�����û���������

};

