#include"stdafx.h"
#include<VersionHelpers.h>
#include"Pipe.h"
using namespace std;
int Player::downloadMessage(char msg[256])
{
	while (1)
	{
		memset(this->buf, 0, sizeof(this->buf));
		GetExitCodeProcess(pi.hProcess, &exit);      //监测子进程是否还存在
		if (exit != STILL_ACTIVE)
		{
			this->PipeStatus = 0;
			return -1;
		}
		PeekNamedPipe(this->read_stdout, this->buf, 255, &this->bytRead, &this->bytAvail, NULL);
		//预览一下有没有数据要读
		if (bytRead != 0)
		{
			memset(this->buf, 0, sizeof(this->buf));
			ReadFile(read_stdout, buf, 255, &bytRead, NULL);
				//printf("%s", buf);
			memcpy_s(msg,sizeof(this->buf),this->buf,sizeof(this->buf));
			break;
		}
		Sleep(500);
	}
	ofstream fcout;
	fcout.open("communitionlog.txt", ios::app);
	fcout << "RX:" << msg << endl;
	fcout.close();
	return 0;
}

int Player::uploadMessage(char* command)
{
	ofstream fcout;
	fcout.open("communitionlog.txt", ios::app);
	fcout <<"TX:"<< command << endl;
	fcout.close();

	GetExitCodeProcess(pi.hProcess, &exit);
	if (exit != STILL_ACTIVE)
	{
		//ErrorHandler();
		this->PipeStatus = 0;
		return -1;
	}
	for (int i = 0; command[i] != 0; i += 1)
	{
		*buf = command[i];
		WriteFile(write_stdin, buf, 1, &bytRead, NULL);
	}
	memset(this->buf, 0, sizeof(this->buf));
	return 0;
}

int Player::StatuCheck(void)
{
	char checkcmd[20];
	char result[256];
	sprintf_s(checkcmd, "BRIDGEVER 1.0\n");
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(checkcmd);
		this->downloadMessage(result);
		if (result[0] == 'O')
		return 0;
	}
		return -1;
}

void Player::closepipe()
{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		CloseHandle(newstdin);
		CloseHandle(newstdout);
		CloseHandle(read_stdout);
		CloseHandle(write_stdin);
}

int Player::UploadInfo(int info[8])
{
	char InfoMsg[50];
	char result[256];
	sprintf_s(InfoMsg, "INFO %d,%d,%d,%d,%d,%d,%d,%d\n",info[0],info[1],info[2],info[3],info[4],info[5],info[6],info[7]);
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(InfoMsg);
		this->downloadMessage(result);
		//printf("%s%s", InfoMsg, result);
		if (result[0] == 'O')
		return 0;
	}

		return -1;
}

int Player::UploadDeal(int posi, int cards[13])
{
	char position = this->ToPositionLetter(posi);
	char DealMsg[50];
	char result[256];
	sprintf_s(DealMsg, "DEAL %c%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", position,cards[0], cards[1], cards[2], cards[3], cards[4], cards[5], cards[6], cards[7],cards[8],cards[9], cards[10], cards[11], cards[12]);
	
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(DealMsg);
		this->downloadMessage(result);
		//printf("%s%s", DealMsg, result);
		if (result[0] == 'O')
			return 0;
	}

		return -1;
}

int Player::UploadDummy(int posi, int cards[13])
{
	char position = this->ToPositionLetter(posi);
	char DummyMsg[50];
	char result[256];
	sprintf_s(DummyMsg, "DUMMY %c%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", position, cards[0], cards[1], cards[2], cards[3], cards[4], cards[5], cards[6], cards[7], cards[8], cards[9], cards[10], cards[11], cards[12]);
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(DummyMsg);
		this->downloadMessage(result);
		//printf("%s%s", DummyMsg, result);
		if (result[0] == 'O')
		return 0;
	}
		return -1;

}

int Player::InquireBid()
{
	//这个函数只返回两位
	char BidInquireMsg[50];
	char result[256];
	sprintf_s(BidInquireMsg, "BID WHAT\n");
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(BidInquireMsg);
		this->downloadMessage(result);
		//printf("%s%s", BidInquireMsg, result);
		if (result[0] == 'B')
			return ((result[5]-48)*10+(result[6]-48));
	}
	return -1;
}

int Player::UploadBid(int bid)
{
	char BidMsg[50];
	char result[256];
	sprintf_s(BidMsg, "BID %c%d%d\n", this->ToPositionLetter(bid/10/10),bid/10%10,bid%10);
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(BidMsg);
		this->downloadMessage(result);
		//printf("%s%s", BidMsg, result);
		if (result[0] == 'O')
		return 0;
	}
		return -1;
}

int Player::UploadContover(int contover)
{
	char ContMsg[50] = {0};
	char result[256] = {0};

	sprintf_s(ContMsg, "CONTOVER %c%d%d%d\n", this->ToPositionLetter(contover / 10 / 10 /10), contover/10 / 10 % 10, contover /10 % 10, contover%10);
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(ContMsg);
		this->downloadMessage(result);
		//printf("%s%s", ContMsg, result);
		if (result[0] == 'O')
		return 0;
	}

		return -1;
}

int Player::InquirePlay(int posi)
{
	char PlayInquireMsg[50];
	char result[256];
	sprintf_s(PlayInquireMsg, "PLAY %cWHAT\n",this->ToPositionLetter(posi));
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(PlayInquireMsg);
		this->downloadMessage(result);
		//printf("%s%s", PlayInquireMsg, result);
		if (result[0] == 'P')
		{
			int card=-1;
			card = (result[7]==13) ? (result[6] - 48) : ((result[6] - 48) * 10 + (result[7] - 48));
			//printf("这个诡异的数字是%d\n", card);
			return card;
		}
	}
		return -1;

}

int Player::UploadPlay(int posi, int card)
{
	char PlayMsg[50];
	char result[256];
	sprintf_s(PlayMsg, "PLAY %c%d\n", this->ToPositionLetter(posi), card);
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(PlayMsg);
		this->downloadMessage(result);
		//printf("%s%s", PlayMsg, result);
		if (result[0] == 'O')
		return 0;
	}

		return -1;
}

int Player::UploadOverMsg(int gameresult)
{
	//result的格式就是百位赢家方位剩下两位墩数好了
	char OverMsg[50];
	char result[256];

	//printf("原%d\n", gameresult);
	//printf("墩%d\n", gameresult - ((gameresult / 100) * 100));


	sprintf_s(OverMsg, "GAMEOVER  %c%d\n", this->ToPositionLetter(gameresult /100), gameresult -((gameresult /100)*100));
	if (this->PipeStatus == 1)
	{
		this->uploadMessage(OverMsg);
		this->downloadMessage(result);
		//printf("%s%s", OverMsg, result);
		if (result[0] == 'O')
			return 0;
	}

		return -1;
}
	
void Player::ErrorHandler(void) 
{
	LPCWSTR msg;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPWSTR)&msg,
		0,
		NULL
					);
	//printf("info: %s\n", msg);
	MessageBox(NULL, msg, TEXT("Pipe Error"), MB_OK);
	LocalFree((HLOCAL)msg);
}

Player::Player(LPCWSTR path)
{
	
	if (IsWindowsXPOrGreater())        //初始化管道安全设置
	{
		InitializeSecurityDescriptor(&this->sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&this->sd, true, NULL, false);
		this->sa.lpSecurityDescriptor = &this->sd;
	}
	else 
		this->sa.lpSecurityDescriptor = NULL;

	if (this->aiJobObject == NULL)
	{
		::MessageBox(0, _T("Could not create job object"), _T("Error"), MB_OK);
		//this->ErrorHandler();
		return;
	}

	this->jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	if (0 == SetInformationJobObject(aiJobObject, JobObjectExtendedLimitInformation, &this->jeli, sizeof(this->jeli)))
	{
		::MessageBox(0, _T("Could not SetInformationJobObject"), _T("Error"), MB_OK);
		return;
	}

	this->sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	this->sa.bInheritHandle = true;         //允许继承

	if (!CreatePipe(&this->newstdin, &this->write_stdin, &this->sa, 0))   //创建输入管道
	{
		this->ErrorHandler();
		return;
	}
	if (!CreatePipe(&this->read_stdout, &this->newstdout, &this->sa, 0))  //创建传出管道
	{
		this->ErrorHandler();
		CloseHandle(newstdin);
		CloseHandle(write_stdin);
		return;
	}
	GetStartupInfo(&this->si);      //set startupinfo for the spawned process
							  /*
							  The dwFlags member tells CreateProcess how to make the process.
							  STARTF_USESTDHANDLES validates the hStd* members. STARTF_USESHOWWINDOW
							  validates the wShowWindow member.
							  */

	this->si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	this->si.wShowWindow = SW_HIDE;
	this->si.hStdOutput = this->newstdout;
	this->si.hStdError = this->newstdout;     //为子进程设置句柄
	this->si.hStdInput = this->newstdin;
	//char app_spawn[] = "";//设置子进程
	if (!CreateProcess(path, NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE,
		NULL, NULL, &this->si, &this->pi))
	{
		this->ErrorHandler();
		//getch();
		CloseHandle(newstdin);
		CloseHandle(newstdout);
		CloseHandle(read_stdout);
		CloseHandle(write_stdin);
		return;
	}
	if (0 == AssignProcessToJobObject(this->aiJobObject, this->pi.hProcess))
	{
		::MessageBox(0, _T("Could not AssignProcessToObject"), _T("Error"), MB_OK);
		return;
	}
	this->PipeStatus = 1;
}

Player::~Player()
{

	TerminateProcess(pi.hProcess, 0);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(newstdin);
	CloseHandle(newstdout);
	CloseHandle(read_stdout);
	CloseHandle(write_stdin);
}

char Player::ToPositionLetter(int num)
{
	switch (num)
	{
	case 0:
		return 'N';
	case 1:
		return 'E';
	case 2:
		return 'S';
	case 3:
		return 'W';
	default:
		return 0;
	}
}
