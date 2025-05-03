#include "pch.h"
#include "list.h"
#include "uboo.h"
#include "tool.h"

const int w = 90;
const int h = 110;
const int NearDistan = 50;
const int XRK_TIME_DISAPEAR = 12 * 1000;
const int XRK_TIME = 15*1000;
const int WD_TIME = 5 * 1000;
const int ZB_RAND_TIME = 20 * 1000;
const int ZB_MOVE = 1;
const int ZB_NUM = 3;
const int FIRE_MOVE = 10;
_pair getPos(int x,int y) {
	return { 100 + 95 * x,110 + 130 * y };
}
int getYcol(int i) {
	return 110 + 130 * i + h / 2;
}
void drawFillRect() {
	setfillcolor(GREEN);
	for (int i = 0;i<5; i++) {
		for (int j = 0; j < 9;j++) {
			solidrectangle(
				100 + 95 * j, 110 + 130 * i,
				100 + 95 * j + 90, 110 + 130 * i + 110
			);
		}
	}
}
int ChoseCard(ExMessage msg) {
	if (isInRect(275,0,275+W/2,100,msg)) {
		if (isInRect(275, 10, 275 + 55, 10 + 75, msg)) {
			return XRK;
		}
		if (isInRect(275 + 65, 10, 275 + 55 + 65, 10 + 75, msg)) {
			return WD;
		}
	}
	return 0;
}
_pair ChoseRect(ExMessage msg) {
	for (int i = 0; i < 5;i++) {
		for (int j = 0; j < 9; j++) {
			if (isInRect(getPos(j, i), {w,h},msg)) {
				return { j,i };
			}
		}
	}
	return { -1,-1 };
}
_pair PlantPos(_pair pos) {
	_pair p = getPos(pos.x, pos.y);
	p.x += w / 2;
	p.y += h / 2;
	return p;
}
int FindNearPlant(List plist,_pair pos) {
	Plant* plant;
	for (int i = 0; i < ListSize(plist);i++) {
		plant = cast(Plant*, ListIndex(plist, i));
		if ( plant->imgid!=20 && distan(PlantPos(plant->pos), pos) < NearDistan) {
			return i;
		}
	}
	return -1;
}
int FindNearZb(List plist, _pair pos) {
	zb* pzb;
	for (int i = 0; i < ListSize(plist); i++) {
		pzb = cast(zb*, ListIndex(plist, i));
		if (distan(pzb->pos, pos) < NearDistan) {
			return i;
		}
	}
	return -1;
}
int NearDoor(_pair door,_pair w) {
	_pair p = PlantPos(door);
	p.y -= 130;
	if (distan(p,w)<=NearDistan) {
		return 1;
	}
	p.y += 130;
	if (distan(p,w)<=NearDistan) {
		return 1;
	}
	p.y += 130;
	if (distan(p, w) <= NearDistan) {
		return 1;
	}
	return 0;
}
int FindNearDoor(List plist,_pair pos,_pair *dst) {
	Plant* plant;
	_pair p;
	int i = 0;
	for (; i < ListSize(plist); i++) {
		plant = cast(Plant*, ListIndex(plist, i));
		if (plant->imgid==20) {
			if (NearDoor(plant->pos,pos)) {
				if (dst) {
					p = PlantPos(plant->pos);
					LOG("Door", "Op Found");
					dst->y = p.y;
					dst->x = p.x + NearDistan * 2;
				}
				return 1;
				break;
			}
		}
	}
	return 0;
}
char mapPos[5][9] = { 0 };

void RandSunShine(List plist,int i=0) {
	static clock_t pre;
	static int Gap = 10;
	sunshine p;
	if (i) {
		pre = clock();
	}
	else {
		if (clock() - pre > Gap * 1000 && ListSize(plist) < 5) {
			p.pos = { rand() % 600 + 100,rand() % 600 + 100 };
			p.pre = clock();
			LOG("NatureSunshine","UP");
			ListPushbacki(plist, &p, sizeof(sunshine));
			Gap += 5;
			if (Gap > 30) {
				Gap = 30;
			}
			pre = clock();
		}
		if (ListSize(plist)>3) {
			pre = clock();
		}
	}
}

void mainGame() {
	int sunShine = 100,MouseChoice=0, isCanPlant = 0,wd_index=0,xrk_index=0,zb_index=0;
	int zb_num = ZM_NUM;
	_pair MouseChoicePlant, predPlant, pos,posxy,Fire,*ppair;
	List listPlant = ListInit(),
		listwd = ListInit(),
		listsunshine = ListInit(),
		listZb = ListInit(),
		listFire = ListInit();
	zb Zb,*pzb;
	sunshine sun,*psun;
	Plant plant,*pplant;
	clock_t pre = clock(),zb_rand=45*1000;
	//putimage(-200, 0, &g_img.Map);//µØÍ¼
	//putItem({ 100,100 }, 0, ZM);
	//while (1);
	RandSunShine(0, 1);
	ExMessage msg;
	BeginBatchDraw();
	while (zb_num+ListSize(listZb)) {
		putimage(-200, 0, &g_img.Map);//µØÍ¼
		putimg((W - g_img.bar.getwidth()) / 2, 0, &g_img.bar);//bar
		putimg(275 + 65 * 0, 10, &g_img.Card_xrk);//¿¨Æ¬ÏòÈÕ¿û
		putimg(275 + 65 * 1, 10, &g_img.Card_wd);//¿¨Æ¬Íã¶¹
		putimg(275 + 65 * 2, 10, &g_img.Card_h);//¿¨Æ¬Íã¶¹
		drawLabeli({ 185,70 }, {60,20}, sunShine,20);
		RandSunShine(listsunshine);
		isCanPlant = 0;
		if (MouseChoice) {
			putItem(MouseChoicePlant, 0, MouseChoice,255);
			predPlant = ChoseRect(msg);
			if (predPlant.x!=-1 && mapPos[predPlant.y][predPlant.x]==0) {
				posxy = predPlant;
				pos = getPos(predPlant.x, predPlant.y);
				pos.x += w / 2;
				pos.y += h / 2;
				putItem(pos, 0, MouseChoice, 128);
				isCanPlant = 1;
			}
		}
		if (clock() - zb_rand >= ZB_RAND_TIME && zb_num) {
			zb_rand = clock();
			Zb.pos = { W,getYcol(rand() % 5) };
			//Zb.pos = { W,getYcol(0) };
			Zb.HP = 200;
			Zb.CanMove = 1;
			Zb.index = 0;
			Zb.pre = clock();
			ListPushbacki(listZb, &Zb, sizeof(zb));
			zb_num--;
			LOG("ZB", "x:%d y:%d", Zb.pos.x, Zb.pos.y);
		}
		if (peekmessage(&msg, EX_MOUSE)) {
			if (msg.lbutton) {
				if (isInRect(275,10,275+55,10+75,msg)) {
					if (sunShine>=50) {
						MouseChoice = XRK;
					}
				}
				else if (isInRect(275 + 65, 10, 275 + 55 + 65, 10 + 75, msg)) {
					if (sunShine>=100) {
						MouseChoice = WD;
					}
				}
				else if (isInRect(275 + 65 * 2, 10, 275 + 55 + 65 * 2, 10 + 75, msg)) {
					if (sunShine >= 75) {
						MouseChoice = 20;
					}
				}
				else if (isCanPlant) {
					isCanPlant = 0;
					plant.pos = posxy;
					plant.imgid = MouseChoice;
					plant.HP = 100;
					plant.pre = clock();
					mapPos[posxy.y][posxy.x] = 1;
					ListPushbacki(listPlant, &plant, sizeof(Plant));
					if (MouseChoice==WD) {
						sunShine -= 100;
					}
					else if (MouseChoice==XRK) {
						sunShine -= 50;
					}
					else if (MouseChoice == 20) {
						sunShine -= 75;
					}
					MouseChoice = 0;
				}

				for (int i = 0; i < ListSize(listsunshine);i++) {
					psun = cast(sunshine*, ListIndex(listsunshine, i));
					if (isInRect(psun->pos, { g_img.Sunshine.getwidth() ,g_img.Sunshine.getheight() }, msg)) {
						sunShine += 25;
						ListDelete(listsunshine, i);
						LOG("sunShineDel","ListSize %d",ListSize(listsunshine));
						break;
					}
				}
			}
			if (msg.rbutton) {
				MouseChoice = 0;
			}
			if (msg.message==WM_MOUSEMOVE) {
				MouseChoicePlant.x = msg.x;
				MouseChoicePlant.y = msg.y;
			}
		}
		//Ö²Îï»æÖÆÒÔ¼°Çé¿öÅÐ¶Ï
		for (int i = 0; i < ListSize(listPlant); i++)
		{
			pplant = cast(Plant*, ListIndex(listPlant, i));
			if (pplant->HP>0) {
				pos = getPos(pplant->pos.x, pplant->pos.y);
				pos.x += w / 2;
				pos.y += h / 2;
				putItem(pos, pplant->imgid == WD ? wd_index : xrk_index, pplant->imgid);
				if (pplant->imgid==XRK && clock()-pplant->pre>XRK_TIME) {
					pplant->pre = clock();
					sun.pos = getPos(pplant->pos.x, pplant->pos.y);
					sun.pos.x += w / 2;
					sun.pos.y += h / 2;
					sun.pre = clock();
					ListPushbacki(listsunshine, &sun, sizeof(sunshine));
				}
				else if (pplant->imgid==WD && clock()-pplant->pre> WD_TIME) {
					pplant->pre = clock();
					Fire = getPos(pplant->pos.x, pplant->pos.y);
					Fire.x += w * 3 / 4;
					Fire.y += h / 2;
					ListPushbacki(listFire, &Fire, sizeof(_pair));
				}
			}
			else {//É¾³ýÖ²Îï

			}
		}
		for (int i = 0; i < ListSize(listsunshine);i++) //Ñô¹â¸üÐÂ
		{
			psun = cast(sunshine*, ListIndex(listsunshine, i));
			putimg(psun->pos.x, psun->pos.y, &g_img.Sunshine);
			if (clock()-psun->pre>XRK_TIME_DISAPEAR) {
				ListDelete(listsunshine, i);
				i--;
			}
		}
		for (int i = 0; i < ListSize(listZb);i++) {//½©Ê¬»æÖÆ
			pzb = cast(zb*, ListIndex(listZb, i));
			putItem(pzb->pos, pzb->index, ZM);
			if (pzb->HP > 0) {
				if (clock() - pzb->pre > 90) {
					pzb->pre = clock();
					pzb->index++;
					if (pzb->index >= ZM_NUM) {
						pzb->index = 0;
					}
					if (FindNearPlant(listPlant, pzb->pos) < 0) {
						if (pzb->pos.x > 100) {
							pzb->pos.x -= ZB_MOVE;
						}
						else {
							LOG("zb", "x:%d", pzb->pos.x);
							MessageBox(GetHWnd(), "Ê§°Ü", "sys", MB_OK);
						}
					}
					else {
						pplant = cast(Plant*, ListIndex(listPlant, FindNearPlant(listPlant, pzb->pos)));
						pplant->HP -= 1;
						if (pplant->HP <= 0) {
							mapPos[pplant->pos.y][pplant->pos.x] = 0;
							ListDelete(listPlant, FindNearPlant(listPlant, pzb->pos));
						}
					}
				}
			}
			else {
				ListDelete(listZb, i);
				i--;
			}
		}
		for (int i = 0; i < ListSize(listFire);i++) {//Íã¶¹
			ppair = cast(_pair*, ListIndex(listFire, i));
			setfillcolor(GREEN);
			fillcircle(ppair->x, ppair->y, 10);
			if (FindNearZb(listZb,*ppair)>=0) {
				cast(zb*, ListIndex(listZb, FindNearZb(listZb, *ppair)))->HP -= 50;
				LOG("ZB", "HP:%d",cast(zb*, ListIndex(listZb, FindNearZb(listZb, *ppair)))->HP);
				ListDelete(listFire,i);
				i--;
				continue;
			}
			else if (FindNearDoor(listPlant,*ppair,0)) {
				LOG("Door","ISR");
				FindNearDoor(listPlant, *ppair, ppair);
			}
		}
		if (clock()-pre>50) {
			pre = clock();
			wd_index++;
			xrk_index++;
			if (wd_index>=WD_NUM) {
				wd_index = 0;
			}
			if (xrk_index>=XRK_NUM) {
				xrk_index = 0;
			}
			for (int i = 0; i < ListSize(listFire); i++) {
				ppair = cast(_pair*, ListIndex(listFire, i));
				ppair->x += FIRE_MOVE;
				if (ppair->x>=W) {
					ListDelete(listFire, i);
					i--;
				}
			}
		}
		Sleep(1);
		FlushBatchDraw();
	}
	EndBatchDraw();
	MessageBox(GetHWnd(),"¹ý¹Ø","sys",MB_OK);
}