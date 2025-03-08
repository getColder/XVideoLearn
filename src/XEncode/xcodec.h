#pragma once
//������

#include <mutex>
#include <vector>

struct AVCodecContext;
struct AVPacket;
struct AVFrame;
void PrintErr(int err);

class XCodec
{

public:
	/////////////////////////////////////////////
	/// ����������������
	///	@para codec_id ������ID�ţ����ֶ�Ӧffmpeg
	/// @return �����������ģ�ʧ�ܷ���NULL
	/////////////////////////////////////////////
	static AVCodecContext* Create(int codec_id, bool is_encode = true);
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
	// ����һ��AVFrame�� �̰߳�ȫ
	/// @return ʧ�ܷ�Χnullptr ����AVPacketҪ�ֶ�����
	/////////////////////////////////////////////
	AVFrame* CreateFrame();

protected:
	AVCodecContext* ctx_ = nullptr;	//������������
	std::mutex mtx_;
};

