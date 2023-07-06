#include "CRtmp.h"
//初始化封装其
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
//添加视频流
bool CRtmp::AddStream(const AVCodecContext* c)
{
	if (!c) return false;
	//b 添加视频流 
	AVStream* st = avformat_new_stream(ic, NULL);
	if (!st)
	{
		cout << "avformat_new_stream failed" << endl;
		return false;
	}
	st->codecpar->codec_tag = 0;
	//从编码器复制参数
	avcodec_parameters_from_context(st->codecpar, c);
	av_dump_format(ic, 0, url.c_str(), 1);
	if (c->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		vc = c;
		vs = st;
	}
	return true;
}
//发送封装头
bool CRtmp::SendHead()
{
	///打开rtmp 的网络输出IO
	int re = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}

	//写入封装头
	re = avformat_write_header(ic, NULL);//会将vs的 time_base 改掉
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}
//推流
bool CRtmp::SendFrame(AVPacket* pack)
{
	if (pack->size <= 0 || !pack->data)return false;
	//推流
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
