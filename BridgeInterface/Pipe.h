#pragma once
#include<windows.h>
using namespace std;
//��������д�ܵ����˹�������
//2018��7��2��
/*================================
fix bugs

����������������Э��ķ���
��ԭ���Ļ��������ϴ������ر�
Ϊ˽�з���ֻ�������Э��ķ�
�����в���

������һЩ�������õ�״̬��⹦��

ʱ��ִ� ��δ��ʱ��д�쳣
������Щ
=================================*/
class Player
{
private:
	unsigned long exit = 0;  //�˳���״̬��
	unsigned long bytRead;   //Ҫ�����ֽ���
	unsigned long bytAvail;   //�����ֽ���
	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;               //�ܵ���ȫ��Ϣ��
	PROCESS_INFORMATION pi;
	HANDLE newstdin, newstdout, read_stdout, write_stdin;  //�ܵ������
	
	int downloadMessage(char msg[256]);	//���������AI�Ļظ��Ž������н��յ��ַ���
	int uploadMessage(char* command);	//�ϴ�����
	void closepipe();
	void ErrorHandler(void);
	char buf[256];   //������������Ļ���
	//�����ǹܵ���ʵ��
	//������Э��Ĳ���
	int PipeStatus;//1����

public:
	//�������к������ǳɹ�����0
	int UploadInfo(int info[8]);//��λ���� Э�����Info�İ��� �ɹ�����0
	int UploadDeal(int posi, int cards[13]);//һλ������� ʮ��λ�ƺ�
	int UploadDummy(int posi, int cards[13]);//һλ������� ʮ��λ�ƺ�
	int InquireBid();//�ɹ�����һ����λ�� ʮλ�ǽ�����1-9�� ��λ�ǻ�ɫ��0-5��
	int UploadBid(int bid);//�ϴ�һ����λ�� ��λ��λ ʮλ������1-9�� ��λ��ɫ��0-5���ɹ�����0
	int UploadContover(int contover);//�ϴ�һ����λ�� ǧλ��λ ��λ������1-9�� ʮλ��ɫ��0-5����λ�ӱ�����ɹ�����0
	int InquirePlay(int posi);//������һλ��λ����
	int UploadPlay(int posi, int card);//������һλ��λ���� �Ʊ���
	int UploadOverMsg(int result);//���� ��λ�� ��λӮ�ҷ�λ ʣ����λӮ����
	
	int StatuCheck(void);//״̬��飬ֻ���ڴ������Ӻ����
	char ToPositionLetter(int num);//�� 0 1 2 3ת��Ϊ NESW
	Player(LPCWSTR path);//���캯�� ����aiĿ¼λ�� 
	~Player();
};