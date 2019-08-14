#pragma once
#include<windows.h>
using namespace std;
//看我来重写管道类了哈哈哈哈
//2018年7月2日
/*================================
fix bugs

添加了若干针对桥牌协议的方法
将原来的基础操作上传和下载变
为私有方法只能由针对协议的方
法进行操作

加入了一些毫无卵用的状态检测功能

时间仓促 并未有时间写异常
处理这些
=================================*/
class Player
{
private:
	unsigned long exit = 0;  //退出的状态码
	unsigned long bytRead;   //要读的字节数
	unsigned long bytAvail;   //可用字节数
	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;               //管道安全信息们
	PROCESS_INFORMATION pi;
	HANDLE newstdin, newstdout, read_stdout, write_stdin;  //管道句柄们
	
	int downloadMessage(char msg[256]);	//下载命令，将AI的回复放进参数中接收的字符串
	int uploadMessage(char* command);	//上传命令
	void closepipe();
	void ErrorHandler(void);
	char buf[256];   //这是输入输出的缓存
	//上面是管道的实现
	//下面是协议的操作
	int PipeStatus;//1正常

public:
	//以下所有函数都是成功返回0
	int UploadInfo(int info[8]);//八位数组 协议里的Info的八项 成功返回0
	int UploadDeal(int posi, int cards[13]);//一位方向代码 十三位牌号
	int UploadDummy(int posi, int cards[13]);//一位方向代码 十三位牌号
	int InquireBid();//成功返回一个两位数 十位是阶数（1-9） 个位是花色（0-5）
	int UploadBid(int bid);//上传一个三位数 百位方位 十位阶数（1-9） 个位花色（0-5）成功返回0
	int UploadContover(int contover);//上传一个四位数 千位方位 百位阶数（1-9） 十位花色（0-5）个位加倍情况成功返回0
	int InquirePlay(int posi);//参数：一位方位代码
	int UploadPlay(int posi, int card);//参数：一位方位代码 牌编码
	int UploadOverMsg(int result);//参数 三位数 百位赢家方位 剩下两位赢墩数
	
	int StatuCheck(void);//状态检查，只能在创建连接后调用
	char ToPositionLetter(int num);//将 0 1 2 3转换为 NESW
	Player(LPCWSTR path);//构造函数 传入ai目录位置 
	~Player();
};