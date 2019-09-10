#include"stdafx.h"
#include"DrawTable.h"

//���ǳ������߼����ڵ�λ��
int main(int argv, char* argc[])
{
	//ofstream fcout;
	//fcout.open("communitionlog.txt");
	//fcout.close();

	initgraph(800, 600);   // ������ͼ���ڣ���СΪ 640x480 ����
	//circle(640/2, 480/2, 50); // ��Բ��Բ��(200, 200)���뾶 100
	//setbkcolor(0x0BAD61);
	setbkcolor(0x00C492);
	cleardevice();
	TCHAR input[80];
	InputBox(
		input,
		50,
		_T("�������־�������Ϣ,��ʽ:\nAIcount,AIposi(s),dealer,turnid,turncount,turntime,\nroundid,roundcount,time,vulnerable,seed"),
		_T("Config"),
		_T("3,0,1,-1,2,0,1,4,1200,1,4,20,1,123456"),
		0,
		0,
		true
	);
	//��������ı�ʾ��ʽ ai���� + ��������ai��� e.g. 3,0,1,2,-1 -1����÷�λ����
	int dealer = 0;
	int seed = 0;
	int aiposition = 0;
	int info[8] = { 1,1,6,1200,1,8,15,1 };
	int aiNumber = 0; //ai ����
	int table[4] = { -1 ,-1,-1,-1};
	swscanf_s(input,_T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"), &aiNumber,&table[0], &table[1], &table[2], &table[3], &dealer,&info[1],&info[2],&info[3],&info[4],&info[5] ,&info[6] ,&info[7],&seed);
	
	//��������ַ���������������ai 
	//line(180, 40, 270*2, 40);
	//line(700, 40, 700, 130*2);
	//line(180, 460, 270*2, 460);
	//line(40, 40, 40, 130*2);

	GAME_STATUS game(dealer, seed,info[7], info[1], info[2]);//���Ʒ� ���� �ֿ� ��ǰ�� ����
	//Player ai(_T("./ai.exe")); //��3 ����Ҫɾ��
	Player aiPosi[4] = { _T("./north.exe") ,_T("./east.exe") ,_T("./south.exe") ,_T("./west.exe") };
	//ai����������ai���� ������±��Ƿ�λ
	/*ϣ��������֧�����ɵ�ai��������*/
	CardsImg drawcards;
	TCHAR output[80];
	wsprintf(output,_T("ai0״̬%d,ai1״̬%d,ai2״̬%d,ai3״̬%d"), aiPosi[0].StatuCheck(), aiPosi[1].StatuCheck(), aiPosi[2].StatuCheck(), aiPosi[3].StatuCheck());
	outtextxy(0, 0, output);
	//printf("�������");

	//��Ϸ��������Ϣ
	int cards[52] = { 0 };
	int playerHands[4][13] = { 0 };//��cards[52]��Ķ���һ�� �������ݸ�ʽ����
	//int aicards[13] = { 0 };
	int avalibleCards[13];
	int avalibleBid[8][5];
	//int seed = 123;
	int process = 0;
	int leader = -1;

	//ai.UploadInfo(info);//��3 ����Ҫɾ��
	for (int i = 0; i < 4; i++)
	{
		if (table[i] != -1)
			aiPosi[i].UploadInfo(info);
		else
			continue;
	}

	int playFlag = 0;
	int badBidFlag[4] = { 0 };//������λ���ƴ������
	int bidcheck = -1;

	//��Ҫ����
	while (1)
	{
		process = game.WhatToDo();
		int currentStep = process / 10;
		int currentPlayer = process % 10;
		wsprintf(output,_T("\n��Ϸ״̬��־%2d\n"), process);
		outtextxy(0, 0, output);
		int num = 0;
		int result = -1;
		int score = 0;
		switch (currentStep)
		{
		case 0:
			cleardevice();
			wsprintf(output,_T("����\n"));
			outtextxy(0, 0, output);
			game.DEAL();
			game.GetCards(cards);

			for (int i = 0, a = 0, b = 0, c = 0, d = 0; i < 52; i++)
			{
				switch (cards[i])
				{
				case 0:
					playerHands[0][a++] = i;
					break;
				case 1:
					playerHands[1][b++] = i;
					break;
				case 2:
					playerHands[2][c++] = i;
					break;
				case 3:
					playerHands[3][d++] = i;
					break;
				default:
					MessageBox(NULL, TEXT("����ʧ�ܣ�"), TEXT("Error"), MB_OK);
					return 1;
				}

			}
			//����
			drawcards.DrawCards(cards);
			//�㲥����
			for (int i = 0; i < 4; i++)
			{
				if (table[i] != -1)
					aiPosi[i].UploadDeal(i, playerHands[i]);
				else
					continue;
			}
			break;

		case 1:
			wsprintf(output,_T("����\n"));
			outtextxy(0, 0, output);
			

			for (int i = 1; i < 8; i++)
				for (int j = 1; j < 6; j++)
					avalibleBid[i - 1][j - 1] = i * 10 + j;

			avalibleBid[7][0] = 80;
			avalibleBid[7][1] = 90;
			avalibleBid[7][2] = 0;
			avalibleBid[7][3] = 0;
			avalibleBid[7][4] = 0;


			for (int i = 0; i < (game.nowBid / 10 % 10 - 1); i++)
				for (int j = 0; j < 5; j++)
					avalibleBid[i][j] = -1;//�������ܽе���
			for (int i = 0; i <= (game.nowBid % 10); i++)
				avalibleBid[(game.nowBid / 10 % 10 - 1)%8][i] = -1;//�������ܽе���
			
			drawcards.DrawBids(avalibleBid);

			wsprintf(output,_T("��ǰ%d\n"), game.nowBid);
			outtextxy(0, 0, output);

			/*������ЩҲҪ����*/
			/**
			if (currentPlayer == aiposition && bidFlag>1)
				//���ai֮ǰ�Ѿ������˴���Ľ������� ֱ������pass
			{	//��AI���Ƴ�����һ����
				result = 0;
			}
			*/

			 if (table[currentPlayer] != -1)
			{
				 if (badBidFlag[currentPlayer] > 1)
					 result = 0;
				 else
					result = aiPosi[currentPlayer].InquireBid();
				wsprintf(output,_T("ai����%d\n"), result);
				outtextxy(0, 14, output);
			}
			else
			{
				result = MakeBid(currentPlayer, avalibleBid[0]);
			}

			bidcheck = game.BID(currentPlayer, result / 10, result % 10);
			if (bidcheck == 0)//����Ϸ� ���Ϸ���-1
			{
				//������Ҫ��һ��ѭ�������ļ� ����ʱ��Ҳ��������
				for (int i = 0; i < 4; i++)
					if (table[i] != -1 && currentPlayer != i)
					//if (currentPlayer != aiposition)//�ĳɲ����ڵ�ǰai��λ�� ���Ƶ�ʱ��Ҳ����
					{
						aiPosi[i].UploadBid(result + currentPlayer * 100);
					}
				//���û��ƽ�����ʷ
				drawcards.DrawBided(game.bidCount, game.bidRecoder);
				//Sleep(500);
			}
			else
			{
				badBidFlag[currentPlayer] += 1;
			}
			break;
	
		case 2:
			wsprintf(output,_T("����\n"));
			outtextxy(0, 14, output);
			
			if (aiposition == game.dummy)//����жϽ�ʱ����ɾ����
			{
				wsprintf(output, _T("�÷�λΪ����\n���ڿ��Թر���"));
				outtextxy(0, 0, output);
			}
			if (playFlag % 4 == 0)//�����һ�տ�ʼ
			{
				Sleep(2000);
				leader = currentPlayer;
				cleardevice();			//�ñ���ɫ�����Ļ
				game.GetCards(cards);
				drawcards.DrawCards(cards);
				//printf("======================================================\n");
			}
			if (playFlag == 0)//����Ǹտ�ʼ����
			{
				for (int i = 0 ;i<4 ; i++)
					if (table[i]!= -1)
						aiPosi[i].UploadContover(game.banker * 1000 + game.contover * 100 + game.trump * 10 + game.dbl);
				//���������AI��Լ
			}
			if (playFlag == 1)//������׹�����
			{
				/*
				for (int i = 0, j = 0; i < 52; i++)
				{
					if (cards[i] == game.dummy)
					{
						aicards[j++] = i;
					}
				}
				//����
				*/
				for (int i = 0; i<4; i++)
					if (table[i] != -1)
						aiPosi[i].UploadDummy(game.dummy, playerHands[game.dummy]);
					//else
						//����
			}
			
			for (int i = 0; i < 13; i++)
				avalibleCards[i] = -1;
			
				for (int i = 0; i < 52; i++)
				{
					if (game.cardPosition[0][i] == currentPlayer && game.cardPosition[1][i] == 1)
					{
						avalibleCards[num++] = i;
					}
				}
				drawcards.DrawBided(game.bidCount, game.bidRecoder);
				//��ʼѯ�ʳ���
				
				if (table[currentPlayer] != -1 && (currentPlayer != game.dummy))//ai����
				{
					
					wsprintf(output,_T("�ȴ�ai����...\n"));
					outtextxy(0, 14, output);
					result = aiPosi[currentPlayer].InquirePlay(currentPlayer);
					
				}
				else if ((table[game.banker] != -1) && (currentPlayer == game.dummy))//aiΪ���ֳ���
				{
					wsprintf(output, _T("�ȴ�aiΪ���ֳ���...\n"));
					outtextxy(0, 14, output);
					result = aiPosi[game.banker].InquirePlay(currentPlayer);
					//ai.UploadPlay(process % 10, result);
				}
				else
				{
					result = ChoseCardToPlay(currentPlayer, avalibleCards);
				}
				
				if (game.PLAY(currentPlayer, result / 4 + 2, result % 4) == 0)//���ƺϷ���֤
				{
					//������Ƹ�aiת��������Ϣ
					//if (process % 10 != aiposition || (game.banker == aiposition && process % 10== game.dummy))//����
					for (int i = 0; i<4; i++)
						if (table[i] != -1 && currentPlayer != i)
							aiPosi[i].UploadPlay(currentPlayer, result);//�Ϸ����ϴ���AI������Ϣ


					cleardevice();			//�ñ���ɫ�����Ļ
					game.GetCards(cards);
					drawcards.DrawCards(cards);
					//drawcards.DrawBided(game.bidCount, game.bidRecoder);
					//��ʾ��ǰ�ƾ�
					drawcards.DrawPlayed(leader, game.cardRecorder + (playFlag / 4 * 4));
					playFlag += 1;
				}
				break;
		case 3:
			wsprintf(output,_T("����\n"));
			outtextxy(0, 14, output);
			score = game.GetScore();
			wsprintf(output,_T("score:%d\n"), score);
			outtextxy(0, 14, output);
			MessageBox(NULL, TEXT("�Ծֽ�����"), TEXT("Info"), MB_OK);
			for (int i=0;i<4;i++)
				if(table[i]!=-1)
				aiPosi[i].UploadOverMsg(game.GetResult());
			

			game.SetForNewGame();

			process = 0;
			leader = -1;

			playFlag = 0;
			badBidFlag[0] =  0 ;
			badBidFlag[1] = 0;
			badBidFlag[2] = 0;
			badBidFlag[3] = 0;
			bidcheck = -1;
			game.gameStatus = 0;
			break;
		}
	}
		//delete &ai;
		delete &game;
		return 0;
}

CardsImg::CardsImg(void)
{
	//ͼ�����
	for (int i = 0; i < 52; i++)
	{
		TCHAR imgname[50];
		wsprintf(imgname, _T("./cards/%d.bmp"), i);
		loadimage(&cards_img[i], imgname);
	}
	for (int i = 0; i <= 90; i++)
	{
		bid_img[i] = NULL;
	}
	for (int i = 0; i <= 7; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			TCHAR imgname[50];
			wsprintf(imgname, _T("./bids/%d.jpg"), i * 10 + j);
			loadimage(&bid_img[i * 10 + j], imgname);
		}
	}
	//��ӱ����ټӱ����Ͳ��ܽе�ͼ��
	loadimage(&bid_img[01], _T("./bids/-1.jpg"));
	loadimage(&bid_img[80], _T("./bids/80.jpg"));
	loadimage(&bid_img[90], _T("./bids/90.jpg"));
	return;
}

void CardsImg::DrawBids(int avaliableBid[8][5])
{
	for (int i = 1, n = 130; i <= 7; i++)
	{
		for (int j = 1, w = 0; j <= 5; j++)
		{
			if (avaliableBid[i - 1][j] == -1)
				putimage(290 + (w++) * 40, n, &bid_img[01]);
			else
				putimage(290 + (w++) * 40, n, &bid_img[i * 10 + j]);
			if (j == 5)
				n += 40;
		}
	}
	putimage(290, 410, &bid_img[0]);
	putimage(410, 410, &bid_img[80]);
	putimage(450, 410, &bid_img[90]);
	return;

}
void CardsImg::DrawCards(int cards[])
{


	for (int i = 0; i < 13; i++)
	{
		cards_N[i] = -1;
		cards_E[i] = -1;
		cards_S[i] = -1;
		cards_W[i] = -1;
	}
	//�ҳ�4������
	for (int i = 0, q = 0, w = 0, e = 0, r = 0; i < 52; i++)
	{
		//if(cards[i]==-1)

		if (cards[i] == 0)
			cards_N[q++] = i;
		else if (cards[i] == 1)
			cards_E[w++] = i;
		else if (cards[i] == 2)
			cards_S[e++] = i;
		else if (cards[i] == 3)
			cards_W[r++] = i;
		else
		continue;
	}
	
	//��4�����ƶ�Ӧͼ����ص�������
	/*
	for (int i = 0, q = 0, w = 0, e = 0, r = 0; i < 13; i++)
	{
	cards_N_img[i] = cards_img[cards_N[q++]];
	cards_E_img[i] = cards_img[cards_E[w++]];
	cards_S_img[i] = cards_img[cards_S[e++]];
	cards_W_img[i] = cards_img[cards_W[r++]];
	}
	//��ʾ���ƽ��
	for (int i = 0, n = 0, e = 0, w = 0; i < 13; i++)
	{
	putimage(180 + (w++) * 7, 40, &cards_N_img[i]);
	putimage(700, 40 + (w++) * 7, &cards_E_img[i]);
	putimage(180 + (w++) * 7, 460, &cards_S_img[i]);
	putimage(40, 40 + (w++) * 7, &cards_W_img[i]);
	}
	*/
		
	for (int cardsnum = 0, w = 0; cardsnum < 13; cardsnum++)
	{
		if (cards_N[cardsnum] == -1)
			break;
		else
		{
			int card = cards_N[cardsnum];
			putimage(180 + (w++) * 28, 40, &cards_img[card]);
		}
	}

	for (int cardsnum = 0, w = 0; cardsnum < 13; cardsnum++)
	{
		if (cards_E[cardsnum] == -1)
			break;
		else
		{
			int card = cards_E[cardsnum];
			putimage(700, 40 + (w++) * 28, &cards_img[card]);
		}
	}

	for (int cardsnum = 0, w = 0; cardsnum < 13; cardsnum++)
	{
		if (cards_S[cardsnum] == -1)
			break;
		else
		{
			int card = cards_S[cardsnum];
			putimage(180 + (w++) * 28, 460, &cards_img[card]);
		}
	}

	for (int cardsnum = 0, w = 0; cardsnum < 13; cardsnum++)
	{
		if (cards_W[cardsnum] == -1)
			break;
		else
		{
			int card = cards_W[cardsnum];
			putimage(40, 40 + (w++) * 28, &cards_img[card]);
		}
	}
	return;
}
void CardsImg::DrawPlayed(int leader, int cards[4])
{
	int player = leader;
	for(int i=0;i<4;i++)
	{
		if (cards[i] == -1)
			break;
		int card = cards[i];
		switch (player)
		{
		case 0:
			putimage(370, 200, &this->cards_img[card]);
			break;
		case 1:
			putimage(450, 260, &this->cards_img[card]);
			break;
		case 2:
			putimage(370, 300, &this->cards_img[card]);
			break;
		case 3:
			putimage(290, 260, &this->cards_img[card]);
			break;
		default:
			continue;
		}
		player = (player + 1 == 4) ? 0 : player + 1;
	}
	return;
}
void CardsImg::DrawBided(int  bidcount,int bidRecoder[100])
{
	for(int i=0,q = 0,w = 0,e = 0,r = 0;i<bidcount;i++)
	{
		IMAGE pass;
		loadimage(&pass, _T("./bids/01.jpg"));
		if (bidRecoder[i] / 100 == 0)
		{
			if(bidRecoder[i]%100==0)
				putimage(180+(q++)*40, 0, &pass);
			else
				putimage(180+(q++)*40, 0, &bid_img[bidRecoder[i]%100]);
		}
		else
			if (bidRecoder[i] / 100 == 1)
		{
			if (bidRecoder[i] % 100 == 0)
				putimage(760, 40 + (w++) * 40,  &pass);
			else
				putimage(760 , 40 + (w++) * 40, &bid_img[bidRecoder[i] % 100]);
		}
		else
			if (bidRecoder[i] / 100 == 2)
		{
			if (bidRecoder[i] % 100 == 0)
				putimage(180 + (e++) * 40, 540, &pass);
			else
				putimage(180 + (e++) * 40, 540, &bid_img[bidRecoder[i] % 100]);
		}
		else
			if (bidRecoder[i] / 100 == 3)
		{
			if(bidRecoder[i] % 100 == 0)
				putimage(0, 40 + (r++) * 40, &pass);
			else
				putimage(0, 40 + (r++) * 40, &bid_img[bidRecoder[i] % 100]);
		}
	}
}







//һ������߰���Ĳ��Դ���
//////////////////////////pipe ���ܲ���
/*
int info[8] = { 1,1,6,1200,1,8,15,1 };
int deal[13] = {51,50,49,48,47,46,45,44,43,42,41,40,39};
int deald[13] = { 0,1,2,3,4,5,6,7,8,9,10,11,12 };
ai.UploadInfo(info);
ai.UploadDeal(0, deal);
ai.UploadBid(300);
int bid = ai.InquireBid();
//printf("��������������%d", bid);
ai.UploadContover(bid*10+0*1000);
ai.UploadPlay(1, 13);
ai.UploadDummy(2, deald);
int card=ai.InquirePlay(2);
ai.UploadPlay(2, card);
ai.UploadPlay(3, 17);
ai.InquirePlay(0);
ai.UploadOverMsg(13);
*/
////////////////////////////////
/*
cin.get();
return 0;
int seed = 123;
int process = 0;
int cards[52];
//game.GetCards(cards);


*/
///////////////////////���߼�ʾ��
/*
while(1)
{
int process = 0;
process = game.WhatToDo();
printf("%2d", process);

int num = 0;
int result = -1;
int score = 0;
switch (process / 10)
{
case 0:
printf("����\n");
game.DEAL(seed);
break;
case 1:
printf("����\n");
for (int i = 1; i < 8; i++)
for (int j = 1; j < 6; j++)
avalibleBid[i - 1][j - 1] = i * 10 + j;
avalibleBid[7][0] = 80;
avalibleBid[7][1] = 90;
printf("��ǰ%d\n", game.nowBid);
for (int i = 0; i < (game.nowBid / 10 % 10 - 1); i++)
for (int j = 0; j < 5; j++)
avalibleBid[i][j] = -1;//�������ܽе���
for (int i = 0; i < (game.nowBid % 10); i++)
avalibleBid[game.nowBid / 10 % 10 - 1][i] = -1;//�������ܽе���
result=MakeBid(process%10,avalibleBid[0]);
game.BID(process % 10, result / 10, result % 10);
//game.gameStatus = 2;
break;
case 2:
printf("����\n");

for (int i = 0; i < 13; i++)
avalibleCards[i] = -1;
for (int i = 0; i < 52; i++)
{
if (game.cardPosition[0][i] == process % 10 && game.cardPosition[1][i] == 1)
{
avalibleCards[num] = i;
num++;
}
}
result =ChoseCardToPlay(process % 10, avalibleCards);
game.PLAY(process % 10, result / 4 + 2, result % 4);
break;
case 3:
//printf("����\n");
score = game.GetScore();
printf("score:%d", score);
break;
}

}

/*
char buffer[256];
ifstream configFile("./config.txt");
if (!configFile.is_open())
{
MessageBox(NULL, TEXT("Can't open config.txt!"), TEXT("Error"), MB_OK);
}
else
{
while (!configFile.eof())
{
configFile.getline(buffer, 100);
switch (buffer[1])
{
case 'V':
sscanf_s(buffer,"EVENTNAME=%s", game.eventName);
printf("%s\n", game.eventName);
break;
case 'S':
sscanf_s(buffer,"NSNAME=%s", game.NSName);
printf("%s\n", game.NSName);
break;
case 'W':
sscanf_s(buffer,"EWNAME=%s", game.NSName);
printf("%s\n", game.EWName);
break;
case 'I':
sscanf_s(buffer,"AIPOSITION=%d", &game.AIPosition);
printf("%d\n", game.AIPosition);
break;
case 'E':
sscanf_s(buffer,"SEED=%d", &game.seed);
printf("%d\n", game.seed);
break;
case 'B':
sscanf_s(buffer, "PBNFILE=%s", game.pbnName);
printf("%s\n", game.pbnName);
break;
default:
break;
}
//cout << buffer << endl;
}
}
*/
//cin.get();
//game.BID(1, 0, 0);
/*���Խ���

int a, b, c;
while (1)
{
if(game.gameStatus!=1)
printf("ׯ����%d\n", game.banker);
if (game.gameStatus == 2)
printf("��Լ��%d\n", game.nowBid);
printf("������%d�׶�\n", game.gameStatus);
printf("PASS����%d\n", game.passCount);
printf("�ֵ�%d��\n", game.nowPlayerPosition);
printf("��λ ���� ��ɫ\n");
scanf_s("%d%d%d", &a, &b, &c);
game.BID(a, b, c);
}
*/
/*���Է���
game.DEAL(123123);
*/
//game.PLAY(0, 0, 0);

/*���Դ���
game.trump = 5;
game.contover = 1;
game.gameStatus = 2;
game.cardPosition[0][48] = 0;
//�����1nt
int a, b, c;
while (1)
{
printf("������%d�׶�\n", game.gameStatus);
printf("�ֵ�%d��\n", game.nowPlayerPosition);
printf("��λ �Ƶ� ��ɫ\n");
scanf_s("%d%d%d", &a, &b, &c);
game.PLAY(a, b, c);
}
*/
//���Է���
/*
game.gameStatus = 3;
game.banker = 1;
game.dummy = 2;
game.trump = 3;
game.contover = 1;
game.trick[game.banker] = 3;
game.trick[game.dummy] = 3;
game.GetScore();
printf("score:%d", game.score);
printf("game status %d", game.gameStatus);
*/
