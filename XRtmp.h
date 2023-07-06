#pragma once
class AVCodecContext;
class AVPacket;
class XRtmp
{
public:
	//������������
	static XRtmp* GetInstance(unsigned char index = 0);
	//��ʼ����װ��������
	virtual bool Init(const char* url) = 0;
	//�����Ƶ����Ƶ��
	virtual bool AddStream(const AVCodecContext* c) = 0;
	//��RTMP����IO,���ͷ�װͷ
	virtual bool SendHead() = 0;
	//����
	virtual bool SendFrame(AVPacket* pack) = 0;

	virtual ~XRtmp();
protected:
	XRtmp();
};

