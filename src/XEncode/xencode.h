#pragma once
#include <mutex>
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
class XEncode
{
public:
	XEncode() {}

	/////////////////////////////////////////////
	/// ����������������
	///	@para codec_id ������ID�ţ����ֶ�Ӧffmpeg
	/// @return �����������ģ�ʧ�ܷ���NULL
	/////////////////////////////////////////////
	static AVCodecContext* Create(int codec_id);
	/////////////////////////////////////////////
	/// ���ö�������������ģ������Ĵ��ݵ������У���Դ��XEncodeά��
	///	@para c ������������, ���ctx��Ϊ�ս���������Դ
	/////////////////////////////////////////////
	void setContext(AVCodecContext* c);

	/////////////////////////////////////////////
	/// ���ñ�������� �̰߳�ȫ
	/////////////////////////////////////////////
	bool SetOpt(const char* key, const char* val);
	bool SetOpt(const char* key, int val);

	/////////////////////////////////////////////
	/// �򿪱������� �̰߳�ȫ
	/////////////////////////////////////////////
	bool Open();

	/////////////////////////////////////////////
	/// �������ݣ� �̰߳�ȫ
	/// @para frame���û�ά��������const��
	/// @return ʧ�ܷ�Χnullptr ����AVPacket�û�Ҫ�ֶ�����
	/////////////////////////////////////////////
	AVPacket* Encode(const AVFrame* frame);
	/////////////////////////////////////////////
	/// ����һ��AVFrame�� �̰߳�ȫ
	/// @return ʧ�ܷ�Χnullptr ����AVPacketҪ�ֶ�����
	/////////////////////////////////////////////
	AVFrame* CreateFrame();
private:
	AVCodecContext* ctx_ = nullptr;	//������������
	std::mutex mtx_;
};

