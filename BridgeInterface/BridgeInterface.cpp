// BridgeInterface.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include "BridgeInterface.h"
#include"Pipe.h"
#include "time.h"

using namespace std;

GAME_STATUS::GAME_STATUS(int dealer, int seed, int vuln, int nowround, int totalround)//构造函数
{
	this->seed = seed;
	this->vulnerable = vuln;
	strcpy_s(this->NorthName, "%NorthName%");
	strcpy_s(this->SouthName, "%SouthName%");
	strcpy_s(this->EastName, "%EastName%");
	strcpy_s(this->WestName, "%WestName%");
	//this->AIPosition = posi;
	this->dealerPosition = dealer;
	this->nowPlayerPosition = dealer;//发牌方有权先叫牌
	for (int i = 0; i < 100; i++)
		this->bidRecoder[i] = -1;
	this->totalRound = totalround;
	this->nowRound = nowround;
	//this->vulnerable = -1;
	this->gameStatus = 0;
	this->trump = 0;
	this->contover = 0;
	this->playCount = 0;
	this->banker = 0;
	this->bidFlag = FALSE;
	this->passCount = 0;
	this->dbl = 0;
	this->score = 0;
	this->firstAttack = -1;
	this->trick[0] = { 0 };
	this->trick[1] = { 0 };
	this->trick[2] = { 0 };
	this->trick[3] = { 0 };
	this->bidCount = 0;
	memset(this->cardPosition, -1, sizeof(this->cardPosition));
	memset(this->cardRecorder, -1, sizeof(this->cardRecorder));
	//memset(this->NSName, 0, sizeof(this->NSName));
	//memset(this->EWName, 0, sizeof(this->EWName));
	memset(this->pbnName, 0, sizeof(this->pbnName));
	this->nowBid = 0;
	this->dummy = -1;
	this->avaliableBidCount = 0; //非加倍的叫牌次数
	sprintf_s(this->pbnName, "%d.pbn", (int)time(NULL));
}

void GAME_STATUS::SetForNewGame(void)
{
	if (this->nowRound <= this->totalRound)
	{
		this->nowPlayerPosition = this->dealerPosition;
		this->PositionNext();
		this->dealerPosition = this->nowPlayerPosition;//发牌玩家下移
		this->vulnerable = (vulnerable + 1 == 4) ? 0 : vulnerable + 1;
		this->gameStatus = 0; //论据状态置为发牌 从新开始游戏
		this->nowRound += 1;
		for (int i = 0; i < 100; i++)
			this->bidRecoder[i] = -1;
		this->trump = 0;
		this->contover = 0;
		this->playCount = 0;
		this->banker = 0;
		this->bidFlag = FALSE;
		this->passCount = 0;
		this->dbl = 0;
		this->score = 0;
		this->trick[0] = { 0 };
		this->trick[1] = { 0 };
		this->trick[2] = { 0 };
		this->trick[3] = { 0 };
		this->bidCount = 0;
		for (int i = 0; i < 52; i++)
			for (int j = 0; j < 2; j++)
			{
				this->cardPosition[j][i] = -1;
				this->cardRecorder[i] = -1;
			}
		this->nowBid = 0;
	}
	else
		this->gameStatus = 5;//待机状态
}

int GAME_STATUS::PositionNext(void)
{
	this->nowPlayerPosition += 1;
	if (this->nowPlayerPosition == 4)
		this->nowPlayerPosition = 0;
	return this->nowPlayerPosition;
}

int GAME_STATUS::BID(int posi, int rank, int suit)
{
	/*这是叫牌的状态更新函数*/
	/*2019年7月29日 修复了所有bug*/
	this->gameStatus = 1;
	if (this->nowPlayerPosition != posi)//检测方位合法性
	{
		this->ErrorHandler(1);
		return -1;
	}
	if ((rank == 0|| rank ==8|| rank ==9) && suit != 0)//检测叫牌合法性
	{
		this->ErrorHandler(2);
		return -1;
	}
	if (suit == 0 && rank == 0)//检测是否为PASS
	{
		this->passCount += 1;
		this->bidRecoder[this->bidCount] = posi * 100;
		this->bidCount+=1;
		this->avaliableBidCount += 1;
		this->PositionNext();
		if (this->passCount == 4)//四家PASS 牌无效
		{
			this->nowPlayerPosition = this->dealerPosition;//开叫玩家指回发牌者
			this->passCount = 0;
			this->MakePBN();
			this->gameStatus = 3;//直接进入结算阶段
			return 0;
		}

		if (this->passCount == 3 && this->bidFlag)	//三家PASS 叫牌结束 写入参数
		{
			for (int i = 0; this->bidRecoder[i] != -1; i++)
			{
				if (this->nowBid % 10 == this->bidRecoder[i] % 10)
				{
					if (this->bidRecoder[i] / 100 == this->banker - 2 ||
						this->bidRecoder[i] / 100 == this->banker + 2)
					{
						this->banker = (this->banker + 2 < 4) ? (this->banker + 2) : (this->banker - 2); //庄为对家
						break;
					}
				}
			}

			//printf("nowbid %d", this->nowBid);
			this->trump = this->nowBid % 10;		//将牌确定
			this->contover = (this->nowBid-this->nowBid/100*100)/10;		//承诺赢墩确定
			this->dummy = (this->banker + 2 < 4) ? (this->banker + 2) : (this->banker - 2); //取对家的操作
			//printf("订约 \n庄家%d \n明手%d\n花色%d\n阶数%d", this->banker, this->dummy, this->trump, this->contover);
			this->MakePBN();						//每个阶段调用pbn的位置都很重要
			
			this->gameStatus = 2;//状态变为打牌状态
			this->nowPlayerPosition = this->banker;	//玩家指向庄家
			this->PositionNext();					//首攻变为庄家下一家
			this->firstAttack = this->nowPlayerPosition;

			//重置叫牌记录
			/*
			this->bidFlag = FALSE;
			this->passCount = 0;					//PASS计数器清零
			this->nowBid = 0;						//叫牌缓存清零
			this->bidCount = 0;
			this->avaliableBidCount = 0;
			memset(this->bidRecoder, 0, sizeof(this->bidRecoder));
			*/
			return 0;
		}
		return 0;
	}


	if (rank<this->nowBid/10%10||((rank==this->nowBid/10 % 10)&&(suit<=this->nowBid%10)))//叫牌非PASS但不合法
	{
		this->ErrorHandler(2);
		return -1;
	}
	//这就过滤完成了叫牌的非法情况
	//接下来是不管干什么 信息合法之后的标准一套操作
	
	this->bidFlag = TRUE;
	
	if (rank > 0 && rank < 8)//正常叫牌
	{
		this->banker = posi;//banker 怎么定还是问题
		this->nowBid = posi * 100 + rank * 10 + suit;
		this->bidRecoder[this->bidCount] = posi * 100 + rank * 10 + suit;
		this->bidCount += 1;
		this->PositionNext();
		this->passCount = 0;
		//printf("a nowbid %d", this->nowBid);
		this->avaliableBidCount += 1;
	}
	
	else if (rank == 8 || rank == 9)//叫加倍
	{
		this->dbl = rank;
		this->bidRecoder[this->bidCount] = posi * 100 + rank * 10 + suit;
		this->bidCount += 1;
		this->PositionNext();
		this->passCount = 0;
		//this->avaliableBidCount +=1;
	}
	else                     //异常
	{
		this->ErrorHandler(8);
		return -1;
	}

	return 0;
}

int GAME_STATUS::GetResult(void)
{
	//rewrite
	int max = 0;
	int maxposi = 0;
	for (int i = 0; i < 4; i++)
	{
		if (this->trick[i] > max)
		{
			max = this->trick[i];
			maxposi = i;
		}
	}
	return maxposi * 100 + max;
}

int GAME_STATUS::PLAY(int posi, int rank, int suit)
{
	if (this->gameStatus != 2)						//状态判断
	{
		this->ErrorHandler(3);
		return -1;
	}
	if(rank<2||rank>15)								//判断牌合法性
	{
		this->ErrorHandler(6);
		return -1;
	}
	int cardnum = (rank - 2) * 4 + suit;			//转换为牌编码
	if (this->nowPlayerPosition != posi)			//检测方位合法性
	{
		this->ErrorHandler(5);
		return -1;
	}
	if (this->cardPosition[0][cardnum] != posi || this->cardPosition[1][cardnum] != 1)	//出牌合法性
	{
		this->ErrorHandler(4);
		return -1;
	}
	//出牌合法的话...
	this->cardRecorder[this->playCount] = cardnum;
	this->PositionNext();
	this->cardPosition[1][cardnum] = 0;
	if (playCount % 4 == 3)//如果是一墩结束
	{
		//printf("调试信息%d", this->WhichCardWinThisTrick(this->cardRecorder[this->playCount - 3], this->cardRecorder[this->playCount - 2], this->cardRecorder[this->playCount - 1], this->cardRecorder[this->playCount]));
		this->nowPlayerPosition=this->cardPosition[0][this->WhichCardWinThisTrick(this->cardRecorder[this->playCount-3], this->cardRecorder[this->playCount-2], this->cardRecorder[this->playCount-1],this->cardRecorder[this->playCount])];
		this->trick[this->nowPlayerPosition] += 1;
		this->MakePBN();
		//printf("%dwin!============================\n\n", this->nowPlayerPosition);
	}

	this->playCount += 1;
	if (playCount == 52)//如果出牌过程结束
	{
		this->gameStatus = 3;//状态变为结算
	}
	return 0;
}

int GAME_STATUS::WhichCardWinThisTrick(int firstcard, int secondcard, int thirdcard, int forthcard)
{
	//这部分是倪同学写的
	int suit = 1, max;
	//efgh
	max = firstcard / 4;
	int maxplayer = 0;
	suit = firstcard % 4;//f=b%4;g=c%4;h=d%4;
	int cardsquence[4] = { firstcard,secondcard,thirdcard,forthcard };
	for (int i = 1; i<4; i++)
	{
		if (suit == cardsquence[i] % 4)
		{
			if (cardsquence[i] / 4 > max)
			{
				max = cardsquence[i] / 4;
				maxplayer = i;
			}
		}
		else if (cardsquence[i] % 4 == (this->trump-1))//将牌赢墩
		{
			suit = cardsquence[i] % 4;
			max = cardsquence[i] / 4;
			maxplayer = i;
		}
	}
	return cardsquence[maxplayer];
}

int GAME_STATUS::DEAL(void)
{
	srand(this->seed);
	int cardList[52] = { 0 };
	//下面是洗牌过程
	for (int i = 0; i < 13; i++)
	{
		cardList[i] = 0;
		cardList[13 + i] = 1;
		cardList[13 + 13 + i] = 2;
		cardList[13 + 13 + 13 + i] = 3;
	}
	for (int i = 0; i < 52; i++)
	{
		int randnum = rand() % 52;
		//cardList[i] ^= cardList[randnum];
		//cardList[randnum] ^= cardList[i];
		//cardList[i] ^= cardList[randnum];
		int temp = cardList[i];
		cardList[i] = cardList[randnum];
		cardList[randnum]= temp;
		this->cardPosition[1][i] = 1;
	}
	memcpy_s(this->cardPosition,sizeof(this->cardPosition), cardList,sizeof(cardList));
	
	
	this->MakePBN();
	this->gameStatus = 1;
	return 0;
}

int GAME_STATUS::WhatToDo(void)
{
	if (gameStatus == 5)
		return -1;
	else
		return this->gameStatus * 10 + this->nowPlayerPosition;
}

int GAME_STATUS::GetScore(void)
{
	//special thanks to 北科 苏心迪
	if (this->gameStatus != 3)//状态判断
	{
		this->ErrorHandler(7);
		return 0;
	}

	//不是四家Pass的情况 可以算分
	if (this->playCount == 52)
	{
		int score = 0;
		int cont = this->trump * 10 + this->contover * 100 + this->dbl;//定约(十位:12345 cdhsn,/百位 阶/个位089 -XDX
		//加倍这坑还没填
		int dic = this->banker;//庄家方位nesw 0123
		int pier = this->trick[this->banker] + this->trick[this->dummy];//庄家得墩
		int num = this->nowRound;
		num--;
		int ifvul[4][16] = { 0, 1, 0, 1, 1, 0, 1, 0,
							0,	1, 0, 1, 1, 0, 1, 0,
							0, 0, 1, 1, 0, 1, 1, 0,
							1, 1, 0, 0, 1, 0, 0, 1,
							0, 1, 0, 1, 1, 0, 1, 0,
							0, 1, 0, 1, 1, 0, 1, 0,
							0, 0, 1, 1, 0, 1, 1, 0,
							1, 1, 0, 0, 1, 0, 0, 1
		};
		//以上是源程序标记局况的方式 暂时保留

		int vulnerable_flag = 0;//有局标志
		if (this->banker % 2 == 0 && this->vulnerable == 1)
			//南北做庄且南北有局
			vulnerable_flag = 1;

		else if (this->banker % 2 == 1 && this->vulnerable == 2)
			//东西做庄且东西有局
			vulnerable_flag = 1;

		else if (this->vulnerable == 3)
			//双有
			vulnerable_flag = 1;
		else
			vulnerable_flag = 0;

		
		if (pier < cont / 100 + 6)//宕
		{
			int spier = cont / 100 + 6 - pier;
			if (!(cont % 10))//未加倍
				//if (!ifvul[dic][num])//有局
				if (!vulnerable_flag)
					score = 50 * spier;
				else
					score = 100 * spier;
			else//加倍
			{
				score = 300 * spier;
				if (!vulnerable_flag)
					switch (spier)
					{
					case 1:
						score -= 200;
						break;
					case 2:
						score -= 300;
						break;
					default:
						score -= 400;
					}
				else
					score -= 100;
				if (cont % 10 == 9)
					score *= 2;
			}
			score = 0 - score;
		}
		else//超
		{
			int escore = 0;//基本墩分
			int color = (cont % 100) / 10;//花色
			int step = cont / 100;//阶
			int ifdbl = cont % 10;
			int cpier = pier - step - 6;
			switch ((color + 3) / 2)
			{
			case 2:
				escore = 20 * step;
				break;
			case 3:
				escore = 30 * step;
				break;
			case 4:
				escore = 30 * step + 10;
				break;
			}
			if (ifdbl == 8)
				escore *= 2;
			if (ifdbl == 9)
				escore *= 4;

			score += 50;//完成奖分
			if (escore >= 100)//成局
				score += 250 + vulnerable_flag * 200;
			//满贯奖励
			if (cont / 100 == 6)//小满贯
				score += 500 + vulnerable_flag * 250;
			if (cont / 100 == 7)//大满贯
				score += 1000 + vulnerable_flag * 500;
			score += escore;

			//加倍奖励
			if (ifdbl == 8)
				score += 50;
			if (ifdbl == 9)
				score += 100;

			if (cpier)
			{
				switch (ifdbl)
				{
				case 0:
					score += (color + 6) / 3 * 10 * cpier;
					break;
				case 8:
					score += 100 * cpier + 100 * vulnerable_flag * cpier;
					break;
				case 9:
					score += 200 * cpier + 200 * vulnerable_flag * cpier;
					break;
				}
			}
		}
		this->score = score;
	}
	else
	{
		this->score = 0;
	}
	//this->nowPlayerPosition = this->dealerPosition;
	//this->PositionNext();
	//this->dealerPosition = this->nowPlayerPosition;
	this->MakePBN();
	//this->SetForNewGame();
	return this->score;

}

void GAME_STATUS::ErrorHandler(int code)
{
	/*这是异常处理函数*/
	int stage = this->WhatToDo();
	cout << "Error" << stage % 10<< code << endl;

	switch (code)
	{

	case 1:
		MessageBox(NULL, TEXT("BID position not in order!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 2:
		MessageBox(NULL, TEXT("BID command error!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 3:
		MessageBox(NULL, TEXT("Wrong time for play!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 4:
		MessageBox(NULL, TEXT("Illegal played card!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 5:
		MessageBox(NULL, TEXT("Play positon not in order!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 6:
		MessageBox(NULL, TEXT("Play card wrong!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 7:
		MessageBox(NULL, TEXT("Wrong time to calculate score!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 8:
		MessageBox(NULL, TEXT("Error bid rank!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 9:
		MessageBox(NULL, TEXT("Can't open config.txt!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 40:
		MessageBox(NULL, TEXT("toPBNSuit(): unexpect card!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 41:
		MessageBox(NULL, TEXT("toPBNPosi(): unexpect card!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	case 42:
		MessageBox(NULL, TEXT("toPBNPosi(): unexpect bidRecorder!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	default:
		MessageBox(NULL, TEXT("UNKNOWN ERROR!"), TEXT("Error"), MB_OK | MB_SYSTEMMODAL);
		break;
	}
	return;
}

char GAME_STATUS::toPBNRank(int card)
{
	card /= 4;
	switch (card)
	{
	case 9:
		return 'J';//J
		break;
	case 10:
		return 'Q';//Q
		break;
	case 11:
		return 'K';//K
		break;
	case 12:
		return 'A';//A
		break;
	case 8:
		return 'T';//10 
		break;
	default:
		return card + 2+'0' ;
		break;
	}
}
char GAME_STATUS::toPBNSuit(int card)
{
	switch (card % 4)
	{
	case 0://C
		return 'C';
		break;
	case 1://D
		return 'D';
		break;
	case 2://H
		return 'H';
		break;
	case 3://S
		return 'S';
		break;
	default:
		//cout << "GetSuit() thrown a error while handling " << cardnum << endl;
		this->ErrorHandler(40);
		break;
	}
	return 0;
}
char GAME_STATUS::toPBNPosition(int posi)
{
	switch (posi)
	{
	case 0://C
		return 'N';
		break;
	case 1://D
		return 'E';
		break;
	case 2://H
		return 'S';
		break;
	case 3://S
		return 'W';
		break;
	default:
		//cout << "GetSuit() thrown a error while handling " << cardnum << endl;
		this->ErrorHandler(41);
		break;
	}
	return 0;

}

void GAME_STATUS::MakePBN(void)
{
	this->pbn.open(this->pbnName, ios::app);
		//写基础信息 发牌信息
	if (this->gameStatus == 0)
	{
		this->pbn << "[Event \"CGA_Bridge\"]\n[Site \" ? \"]\n[Date \"?\"]\n[Board \"" << this->nowRound << "\"]\n[West \"" << this->WestName << "\"]\n[North \"" << this->NorthName << "\"]\n[East \"" << this->EastName << "\"]\n[South \"" << this->SouthName << "\"]\n";
		this->pbn << "[Dealer \"" << this->toPBNPosition(this->dealerPosition) << "\"]\n";
		switch (this->vulnerable)
		{
		case 0:
			this->pbn << "[Vulnerable  \"" << "None" << "\"]\n";
			break;
		case 1:
			this->pbn << "[Vulnerable  \"" << "NS" << "\"]\n";
			break;
		case 2:
			this->pbn << "[Vulnerable  \"" << "EW" << "\"]\n";
			break;
		case 3:
			this->pbn << "[Vulnerable  \"" << "Both" << "\"]\n";
			break;
		default:
			break;
		}
		//写发牌信息
		int cards[4][4][13];//玩家//花色//牌
		memset(cards, -1, sizeof(cards));
		int flag[4][4] = { 0 };
		//int cardsbefore[4][4] = { 0 };

		for (int i = 0; i < 52; i++)
			cards[this->cardPosition[0][i]][i % 4][flag[this->cardPosition[0][i]][i % 4]++] = i;
		//memcpy_s(cardsbefore, sizeof(cardsbefore), flag, sizeof(flag));
		//for (int i = 0; i < 4; i++)
		//{
		//	cardsbefore[i][1] += cardsbefore[i][0];
		//	cardsbefore[i][2] += cardsbefore[i][1];
		//	cardsbefore[i][3] += cardsbefore[i][2];
		//	cardsbefore[i][0] = 0;
		//}
		//char dealinfo[80];
		//memset(dealinfo, 0, sizeof(dealinfo));
		//for (int i = 0; i < 4; i++)
			//for (int j = 0; j < 4; j++)
				//for (int k = 0; k < flag[i][j]; k++)
					//dealinfo[i*17+cardsbefore[i][j]] = cards[i][j][k];

		this->pbn << "[Deal \"W:";
		for (int i = 3; i >= 0; i--)
		{
		for (int j = 0; j < flag[3][i]; j++)
			this->pbn << this->toPBNRank(cards[3][i][j]);
		if(i>0)
			this->pbn << '.';
		}
		this->pbn << ' ';
		for (int i = 3; i >= 0; i--)
		{
			for (int j = 0; j < flag[0][i]; j++)
				this->pbn << this->toPBNRank(cards[0][i][j]);
			if (i>0)
				this->pbn << '.';
		}
		this->pbn << ' ';
		for (int i = 3; i >= 0; i--)
		{ 
			for (int j = 0; j < flag[1][i]; j++)
				this->pbn << this->toPBNRank(cards[1][i][j]);
			if (i>0)
				this->pbn << '.';
		}
		this->pbn << ' ';
		for (int i = 3; i >= 0; i--)
		{
			for (int j = 0; j < flag[2][i]; j++)
				this->pbn << this->toPBNRank(cards[2][i][j]);
			if (i>0)
				this->pbn << '.';
		}
		this->pbn << "\"]\n";
	}
		//写叫牌信息
	else if (this->gameStatus == 1)
	{
		//写叫牌过程
		this->pbn << "[Auction \"" << this->toPBNPosition(this->dealerPosition) << "\"]\n";
		for (int i = 0; i < this->bidCount; i++)

		{
			int bidCode = this->bidRecoder[i];
			char bid[] = "Pass";
			if ((bidCode - bidCode / 100 * 100) / 10 == 8)
			{
				bid[0] = '=';
				bid[1] = '1';
				bid[2] = '=';
				bid[3] = 0;
			}
			else if ((bidCode - bidCode / 100 * 100) / 10 == 9)
			{
				bid[0] = '=';
				bid[1] = '2';
				bid[2] = '=';
				bid[3] = 0;
			}
			else if ((bidCode - bidCode / 100 * 100) / 10 == 0)
			{
				bid[0] = 'P';
				bid[1] = 'a';
				bid[2] = 's';
				bid[3] = 's';
			}
			else
			{
				bid[0] = ((bidCode - bidCode / 100 * 100) / 10) + '0';
				switch (bidCode % 10)
				{
				case 1:
					bid[1] = 'C';
					bid[2] = 0;
					break;
				case 2:
					bid[1] = 'D';
					bid[2] = 0;
					break;
				case 3:
					bid[1] = 'H';
					bid[2] = 0;
					break;
				case 4:
					bid[1] = 'S';
					bid[2] = 0;
					break;
				case 5:
					bid[1] = 'N';
					bid[2] = 'T';

					break;
				default:
					this->ErrorHandler(42);
					break;
				}
				bid[3] = 0;
			}
			if (i % 4 == 3)
				this->pbn << bid << '\n';
			else
				this->pbn << bid << ' ';
		}
		this->pbn << "\n";

		//写订约结果
		if (this->contover != 0)
		{
			this->pbn << "[Declarer \"" << this->toPBNPosition(this->banker) << "\"]\n";
			char contract[10];
			contract[0] = this->contover + '0';
			switch (this->trump)
			{
			case 1:
				contract[1] = 'C';
				contract[2] = 0;
				break;
			case 2:
				contract[1] = 'D';
				contract[2] = 0;
				break;
			case 3:
				contract[1] = 'H';
				contract[2] = 0;
				break;
			case 4:
				contract[1] = 'S';
				contract[2] = 0;
				break;
			case 5:
				contract[1] = 'N';
				contract[2] = 'T';
				break;
			default:
				this->ErrorHandler(42);
				break;
			}
			switch (this->dbl)
			{
			case 0:
				if (this->trump == 5)
					contract[3] = 0;
				else
					contract[2] = 0;
				break;
			case 8:
				if (this->trump == 5)
				{
					contract[3] = 'X';
					contract[4] = 0;
				}
				else
				{
					contract[2] = 'X';
					contract[3] = 0;
				}
				break;
			case 9:
				if (this->trump == 5)
				{
					contract[3] = 'X';
					contract[4] = 'X';
					contract[5] = 0;
				}
				else
				{
					contract[2] = 'X';
					contract[3] = 'X';
					contract[4] = 0;
				}
				break;
			default:
				this->ErrorHandler(42);
				break;
			}
			this->pbn << "[Contract \"" << contract << "\"]\n";
		}
	}
		//写打牌信息
	else if (this->gameStatus == 2)
	{
		if(this->playCount==3)
			this->pbn << "[Play \"" << this->toPBNPosition(this->firstAttack) << "\"]\n";
		//for (int i = 0; i <= this->playCount / 4; i++)
		int i = this->playCount/4;
		{
			int card_posi_in_recorder = i * 4;
			int cards_in_this_trick[4] = { -1,-1,-1,-1 };
			
			memcpy_s(cards_in_this_trick, sizeof(cards_in_this_trick), this->cardRecorder + card_posi_in_recorder, sizeof(cards_in_this_trick));
			int posi_order[4] = { -1,-1,-1,-1 };
			posi_order[0] = this->firstAttack;
			posi_order[1] = (posi_order[0] +1==4)?0: (posi_order[0] + 1);
			posi_order[2] = (posi_order[1] + 1 == 4) ? 0 :( posi_order[1]+1);
			posi_order[3] = (posi_order[2] + 1 == 4) ? 0 :( posi_order[2] + 1) ;
			
			for (int j = 0; j < 4; j++)
				if (this->cardPosition[0][cards_in_this_trick[j]] == posi_order[0])
					pbn << this->toPBNSuit(cards_in_this_trick[j]) << this->toPBNRank(cards_in_this_trick[j]) << ' ';
			for (int j = 0; j < 4; j++)
				if (this->cardPosition[0][cards_in_this_trick[j]] == posi_order[1])
					pbn << this->toPBNSuit(cards_in_this_trick[j]) << this->toPBNRank(cards_in_this_trick[j]) << ' ';
			for (int j = 0; j < 4; j++)
				if (this->cardPosition[0][cards_in_this_trick[j]] == posi_order[2])
					pbn << this->toPBNSuit(cards_in_this_trick[j]) << this->toPBNRank(cards_in_this_trick[j]) << ' ';
			for (int j = 0; j < 4; j++)
				if (this->cardPosition[0][cards_in_this_trick[j]] == posi_order[3])
					pbn << this->toPBNSuit(cards_in_this_trick[j]) << this->toPBNRank(cards_in_this_trick[j]) << '\n';
		}
	}
		//写结算信息
	else if (this->gameStatus == 3)
	{
		this->pbn << "[Scoring \"IMP\"]\n";
		this->pbn << "[Result \"" << this->trick[this->banker]+this->trick[this->dummy] << "\"]\n";
		if (this->banker%2==0)
		this->pbn << "[Score \"NS " << this->score << "\"]\n";
		else
		this->pbn << "[Score \"EW " << this->score << "\"]\n";
		this->pbn << "*\n\n";
	}
		
	this->pbn.close();
}

void GAME_STATUS::GetCards(int* cards)
{
//将牌的分布情况传入数组中
	for (int i = 0; i < 52; i++)
	{
		if (this->cardPosition[1][i] != 0)
			*(cards + i) = this->cardPosition[0][i];
		else
			*(cards + i) =-1;
	}
}

GAME_STATUS::~GAME_STATUS()
{
}