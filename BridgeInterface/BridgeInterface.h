#pragma once
//#include<fstream>
using namespace std;
class GAME_STATUS
{
private:

	
	//�������������¼����
	int totalRound;				//������
	int nowRound;				//��ǰ����
	

	bool bidFlag;		//�Ƿ����˿���pass
	int passCount;				//PASS����
	int firstAttack;
	
	int avaliableBidCount;		//����ӱ����˼���
	int nowPlayerPosition;		//��ǰ�ֵ������ 0-3 N-W -1E

	void ErrorHandler(int code);
	int PositionNext(void);		//ʹ���е�ǰ���ָ����һ��
	
	char toPBNRank(int card);	
	char toPBNSuit(int card);
	char toPBNPosition(int posi);
	void MakePBN(void);			//��������PBN�ļ�
	int WhichCardWinThisTrick(int firstcard, int secondcard, int thirdcard, int forthcard); //�������
	ofstream pbn;
	
public:
	int gameStatus;				//��Ϸ���� -1 δ֪ 0���� 1���� 2���� 3���� 5����
	int seed;					//����
	
	char eventName[20];
	char NSName[20];
	char EWName[20];
	char pbnName[80];			//pbn�ļ���
	int dealerPosition;			//���Ʒ� 0-3 N-W -1E
	int cardPosition[2][52];	//ֽ�Ʒ�λ ��һ�з�λ �ڶ����Ƿ����� 0-3 N-W 0-1 -1E 		
	int cardRecorder[52];		//���Ƽ�¼ 0-51 CARDCODE
	int bidRecoder[100];		//��һλ��λ �ڶ�λ���� ����λ��ɫ 000����pass 080�����ӱ� 015����1NT 
	int trump;					//����	 1-5
	int contover;				//��ŵӮ��1-7
	int dbl;					//�ӱ���� 0 8 9
	int nowBid;					//��ǰ�е��Ļ�ɫ ��һλ��λ ��һλ���� �ڶ�λ��ɫ 000����pass  015����1NT 
	int vulnerable;				//�ֿ� 0-NEITHER 1-NS 2-EW 3-BOTH -1E
	int banker;					//ׯ��
	int dummy;					//����
	int score;					//����
	int trick[4];				//Ӯ�ռ���	
	
	int playCount;				//���Ƽ���
	int bidCount;				//���Ƽ���

	GAME_STATUS(int dealer, int seed, int vuln, int nowround,int totalround);//���캯�� �����б�Ϊ ���Ʒ� ���� �ֿ� ������
	int WhatToDo(void);			//���ص�ǰ���е��ĸ����裨ʮλ�� �ֵ��Ǹ���ң���λ��
	int GetResult(void);		//����һ����λ�� ��λΪӮ�ҷ�λ ʮλΪ����
	void SetForNewGame(void);	//׼����һ����Ϸ(�ֿ� ���Ʒ� ����һ�ֻ����ϸ� �������)���ﵽ������� ״̬��Ϊ5
	void GetCards(int* cards);	//����������Ƶķ�λ״��
	int BID(int posi,int rank, int suit);						//���ƹ���
	int PLAY(int posi, int rank, int suit);						//���ƹ���//2-A 2-13 C-S 0-3
	int DEAL(void);												//���ƹ��� �����Ѿ��ڹ��캯�����������
	int GetScore(void);											//���
	~GAME_STATUS();			//��������
};

