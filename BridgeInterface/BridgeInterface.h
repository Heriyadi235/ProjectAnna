#pragma once
//#include<fstream>
using namespace std;
class GAME_STATUS
{
private:

	
	//这个数组用来记录出牌
	int totalRound;				//总轮数
	int nowRound;				//当前轮数
	

	bool bidFlag;		//是否还无人开叫pass
	int passCount;				//PASS计数
	int firstAttack;
	
	int avaliableBidCount;		//不算加倍叫了几次
	int nowPlayerPosition;		//当前轮到的玩家 0-3 N-W -1E

	void ErrorHandler(int code);
	int PositionNext(void);		//使类中当前玩家指向下一家
	
	char toPBNRank(int card);	
	char toPBNSuit(int card);
	char toPBNPosition(int posi);
	void MakePBN(void);			//用以生成PBN文件
	int WhichCardWinThisTrick(int firstcard, int secondcard, int thirdcard, int forthcard); //找最大牌
	ofstream pbn;
	
public:
	int gameStatus;				//游戏进程 -1 未知 0发牌 1叫牌 2打牌 3结算 5结束
	int seed;					//种子
	
	char eventName[20];
	char NSName[20];
	char EWName[20];
	char pbnName[80];			//pbn文件名
	int dealerPosition;			//发牌方 0-3 N-W -1E
	int cardPosition[2][52];	//纸牌方位 第一行方位 第二行是否在手 0-3 N-W 0-1 -1E 		
	int cardRecorder[52];		//出牌记录 0-51 CARDCODE
	int bidRecoder[100];		//第一位方位 第二位阶数 第三位花色 000北方pass 080北方加倍 015北方1NT 
	int trump;					//将牌	 1-5
	int contover;				//承诺赢墩1-7
	int dbl;					//加倍情况 0 8 9
	int nowBid;					//当前叫到的花色 第一位方位 第一位阶数 第二位花色 000北方pass  015北方1NT 
	int vulnerable;				//局况 0-NEITHER 1-NS 2-EW 3-BOTH -1E
	int banker;					//庄家
	int dummy;					//明手
	int score;					//分数
	int trick[4];				//赢墩计数	
	
	int playCount;				//出牌计数
	int bidCount;				//叫牌计数

	GAME_STATUS(int dealer, int seed, int vuln, int nowround,int totalround);//构造函数 参数列表为 发牌方 种子 局况 总轮数
	int WhatToDo(void);			//返回当前进行到哪个步骤（十位） 轮到那个玩家（个位）
	int GetResult(void);		//返回一个三位数 百位为赢家方位 十位为墩数
	void SetForNewGame(void);	//准备新一轮游戏(局况 发牌方 在上一轮基础上改 无需参数)若达到最大轮数 状态变为5
	void GetCards(int* cards);	//获得现在手牌的方位状况
	int BID(int posi,int rank, int suit);						//叫牌过程
	int PLAY(int posi, int rank, int suit);						//打牌过程//2-A 2-13 C-S 0-3
	int DEAL(void);												//发牌过程 种子已经在构造函数中配置完成
	int GetScore(void);											//算分
	~GAME_STATUS();			//析构函数
};

