#include "CRtmp.h"
//��ʼ����װ��
bool CRtmp::Init(const char* url)
{
	int re = avformat_alloc_output_context2(&ic, 0, "flv", url);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	this->url = url;
	return true;
}

void CRtmp::close()
{
	if (ic)
	{
		avformat_close_input(&ic);
		vs = NULL;
		url = "";
	}
	vc = NULL;
}
//�����Ƶ��
bool CRtmp::AddStream(const AVCodecContext* c)
{
	if (!c) return false;
	//b �����Ƶ�� 
	AVStream* st = avformat_new_stream(ic, NULL);
	if (!st)
	{
		cout << "avformat_new_stream failed" << endl;
		return false;
	}
	st->codecpar->codec_tag = 0;
	//�ӱ��������Ʋ���
	avcodec_parameters_from_context(st->codecpar, c);
	av_dump_format(ic, 0, url.c_str(), 1);
	if (c->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		vc = c;
		vs = st;
	}
	return true;
}
//���ͷ�װͷ
bool CRtmp::SendHead()
{
	///��rtmp ���������IO
	int re = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}

	//д���װͷ
	re = avformat_write_header(ic, NULL);//�Ὣvs�� time_base �ĵ�
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}
//����
bool CRtmp::SendFrame(AVPacket* pack)
{
	if (pack->size <= 0 || !pack->data)return false;
	//����
	pack->pts = av_rescale_q(pack->pts, vc->time_base, vs->time_base);
	pack->dts = av_rescale_q(pack->dts, vc->time_base, vs->time_base);
	pack->duration = av_rescale_q(pack->duration, vc->time_base, vs->time_base);
	int re = av_interleaved_write_frame(ic, pack);
	if (re == 0)
	{
		cout << "#" << flush;
	}
	return true;
}
