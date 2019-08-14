#pragma once
#include <windows.h>
#include <graphics.h> 
#include "BridgeInterface.h"
#include"Pipe.h"
#include"MakeMove.h"
class CardsImg
{
private:

	
	IMAGE bid_img[91];
	int cards_N[13], cards_E[13], cards_S[13], cards_W[13];

public:
	IMAGE cards_img[52], cards_N_img[13], cards_E_img[13], cards_S_img[13], cards_W_img[13];
	CardsImg(void);
	void DrawCards(int cards[]);
	void DrawBids(void);
	void DrawBided(int  bidcount,int bidRecoder[100]);
	void DrawPlayed(int leader,int cards[4]);
};

