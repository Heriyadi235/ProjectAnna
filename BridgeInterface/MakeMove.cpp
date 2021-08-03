#include"stdafx.h"
#include"makemove.h"
#include "DrawTable.h"
//牌的显示排序一定要和avaliable里排序相同!!!!!!!

void ShowGameInfo() 
{
	return;
}
int ChoseCardToPlay(int posi ,int avaliableCards[13])
{
	int i=0;
	int card=-1;
	TCHAR point[50];
	wsprintf(point,_T("选择%d的出牌\n"), posi);
	outtextxy(0, 14, point);
	//while (avaliableCards[i] >=0)
	//{
		//printf("%2d ", avaliableCards[i]);
	//	i++;
	//}
	//printf("\nchoose one\n");
	//scanf_s("%d",&card);
	
	//int posipixel[4] = { 40,700,460,40 };//NSy EWx
	//接收鼠标读取函数
	flushmessage();
	EASYXMSG move;
	//line(0, 0, 480, 480);
	while (1)
	{//等待鼠标事件
		move = getmessage(EM_MOUSE);
		if (move.lbutton)
		{
			
			card = GetCard(move.x, move.y);
			wsprintf(point, _T("x:%d,y:%dcard:%d    "), move.x, move.y,card);
			outtextxy(0, 0, point);
			
			if (card-(100*card/100) >= 0 && card-(100 * card / 100) < 13&&card/100==posi)
			{
				card  = avaliableCards[card - ( card / 100*100)];
				outtextxy(0, 0, card);
				return card;
				break;
			}
		}
	}
	//sprintf_s(input, "Done!");
	
}
int MakeBid(int posi,int avaliableBid[])
{
	//返回一个两位数 十位表示叫品 个位表示花色
	
	int bid=0;
	/*
	printf("%d have:\n", posi);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			printf("%2d\t", *(avaliableBid + 5*i + j));
		}
		printf("\n");
	}
	printf("\nchoose one\n");
	scanf_s("%d", &bid);
	*/
	//接收鼠标读取函数
	TCHAR point[50];
	wsprintf(point, _T("选择%d的叫牌\n"), posi);
	outtextxy(0, 14, point);
	flushmessage();
	EASYXMSG move;
	//line(0, 0, 480, 480);
	while (1)
	{//等待鼠标事件
		move = getmessage(EM_MOUSE);
		if (move.lbutton)
		{
			bid = GetBid(move.x, move.y);
			
			wsprintf(point,_T("x:%d, y:%d bid:%d    "), move.x, move.y,bid);
			outtextxy(0, 0, point);
			if (bid >= 0)
			{
				break;
			}
		}
	}

	//sprintf_s(input, "Done!");
	outtextxy(0, 0, bid);
	return bid;
}
int GetCard(short x, short y)
{
	//现在有个bug 只能点牌左边一条的区域
	//NS向左搜索
	//EW向上搜索
	int close = 12;

	if (x >= 180 && x <= 575 && y >= 40 && y <= 120)//N的牌
	{
		int posi_x[13] = { 180,207,236,264,292,320,348,376,404,432,460,489,517 };
	
		//for (int i = 0; i < 12; i++)
			//line(posi_x[i], 40, posi_x[i], 40 + 80);
		for (close = 12; close >= 0; close--)
		{
			if ((x - posi_x[close] )>= 0)
				break;
		}
		return close;
	}
	else if (x >= 700 && x <= 760 && y >= 40 && y <= 457)//E的牌
	{
		int posi_y[13] = { 40,68,96,124,152,180,208,236,264,292,320,348,376 };
		//for (int i = 0; i < 12; i++)
			//line(700,posi_y[i], 700+60, posi_y[i]);
		for (close = 12; close >= 0; close--)
		{
			if ((y - posi_y[close]) > 0)
				break;
		}
		return  close+100;
	}
	else if (x >= 180 && x <= 576 && y >= 460 && y <= 540)//S的牌
	{
		int posi_x[13] = { 180,207,236,264,292,320,348,376,404,432,460,489,517 };
		//for (int i = 0; i < 12; i++)
			//line(posi_x[i], 460, posi_x[i], 460 + 80);
		for (close = 12; close >= 0; close--)
		{
			if ((x - posi_x[close]) >= 0)
				break;
		}
		return  close+200;
	}
	else if (x >= 40 && x <= 100 && y >= 40 && y <= 457)//W的牌
	{
		int posi_y[13] = { 40,68,96,124,152,180,208,236,264,292,320,348,376 };
		//for (int i = 0; i < 12; i++)
			//line(40, posi_y[i], 40+60, posi_y[i]);
		for (close = 12; close >= 0; close--)
		{
			if ((y - posi_y[close] )> 0)
				break;
		}
		return  close+300;
	}
	else
		return -1;
}
int GetBid(short x, short y)
//获取坐标对应的叫牌
{


	if (x >= 290 && x <= 400 && y >= 410 && y <= 450)
		return 0;
	else if (x >= 290 && x <= 320 && y >= 130 && y <= 170)
		return 11;
	else if (x >= 320 && x <= 360 && y >= 130 && y <= 170)
		return 12;
	else if (x >= 360 && x <= 400 && y >= 130 && y <= 170)
		return 13;
	else if (x >= 400 && x <= 440 && y >= 130 && y <= 170)
		return 14;
	else if (x >= 440 && x <= 480 && y >= 130 && y <= 170)
		return 15;

	else if (x >= 290 && x <= 320 && y >= 170 && y <= 210)
		return 21;
	else if (x >= 320 && x <= 360 && y >= 170 && y <= 210)
		return 22;
	else if (x >= 360 && x <= 400 && y >= 170 && y <= 210)
		return 23;
	else if (x >= 400 && x <= 440 && y >= 170 && y <= 210)
		return 24;
	else if (x >= 440 && x <= 480 && y >= 170 && y <= 210)
		return 25;

	else if (x >= 290 && x <= 320 && y >= 210 && y <= 250)
		return 31;
	else if (x >= 320 && x <= 360 && y >= 210 && y <= 250)
		return 32;
	else if (x >= 360 && x <= 400 && y >= 210 && y <= 250)
		return 33;
	else if (x >= 400 && x <= 440 && y >= 210 && y <= 250)
		return 34;
	else if (x >= 440 && x <= 480 && y >= 210 && y <= 250)
		return 35;

	else if (x >= 290 && x <= 320 && y >= 250 && y <= 290)
		return 41;
	else if (x >= 320 && x <= 360 && y >= 250 && y <= 290)
		return 42;
	else if (x >= 360 && x <= 400 && y >= 250 && y <= 290)
		return 43;
	else if (x >= 400 && x <= 440 && y >= 250 && y <= 290)
		return 44;
	else if (x >= 440 && x <= 480 && y >= 250 && y <= 290)
		return 45;

	else if (x >= 290 && x <= 320 && y >= 290 && y <= 330)
		return 51;
	else if (x >= 320 && x <= 360 && y >= 290 && y <= 330)
		return 52;
	else if (x >= 360 && x <= 400 && y >= 290 && y <= 330)
		return 53;
	else if (x >= 400 && x <= 440 && y >= 290 && y <= 330)
		return 54;
	else if (x >= 440 && x <= 480 && y >= 290 && y <= 330)
		return 55;

	else if (x >= 290 && x <= 320 && y >= 330 && y <= 370)
		return 61;
	else if (x >= 320 && x <= 360 && y >= 330 && y <= 370)
		return 62;
	else if (x >= 360 && x <= 400 && y >= 330 && y <= 370)
		return 63;
	else if (x >= 400 && x <= 440 && y >= 330 && y <= 370)
		return 64;
	else if (x >= 440 && x <= 480 && y >= 330 && y <= 370)
		return 65;

	else if (x >= 290 && x <= 320 && y >= 370 && y <= 410)
		return 71;
	else if (x >= 320 && x <= 360 && y >= 370 && y <= 410)
		return 72;
	else if (x >= 360 && x <= 400 && y >= 370 && y <= 410)
		return 73;
	else if (x >= 400 && x <= 440 && y >= 370 && y <= 410)
		return 74;
	else if (x >= 440 && x <= 480 && y >= 370 && y <= 410)
		return 75;

	else if (x >= 400 && x <= 440 && y >= 410 && y <= 450)
		return 80;
	else if (x >= 440 && x <= 480 && y >= 410 && y <= 450)
		return 90;
	return -1;
}