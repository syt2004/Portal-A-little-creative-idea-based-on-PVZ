#include "uboo.h"
#include "pch.h"
Img g_img;

void testPos() {
	ExMessage msg;
	while (1) {
		if (peekmessage(&msg, EX_MOUSE)) {
			if (msg.lbutton) {
				LOG("Pos", "x:%d y:%d", msg.x, msg.y);
			}
			else if (msg.rbutton) {
				break;
			}
		}
	}
}

void LoadimageDat() {
	char buff[64];
	loadimage(&g_img.Map, "Dat/res/Map/map0.jpg", W+650, H);
	loadimage(&g_img.bar, "Dat/res/Screen/ChooserBackground.png",W*2/3,100);
	loadimage(&g_img.Card_xrk, "Dat/res/Cards/card_2.png",55,75);
	loadimage(&g_img.Card_wd, "Dat/res/Cards/card_1.png",57,75);
	loadimage(&g_img.Card_h, "Dat/res/Cards/card_h.png", 55, 75);
	loadimage(&g_img.Sunshine, "Dat/res/sunshine/1.png");
	loadimage(&g_img.xuanwo, "Dat/0.png",75,75);
	for (int i = 0; i < ZM_NUM; i++) {
		sprintf(buff, "Dat/res/zm/%d.png", i + 1);
		loadimage(&g_img.zm[i], buff);
	}
	for (int i = 0; i < WD_NUM; i++) {
		sprintf(buff, "Dat/res/zhiwu/0/%d.png", i + 1);
		loadimage(&g_img.wd[i], buff);
	}
	for (int i = 0; i < XRK_NUM; i++) {
		sprintf(buff, "Dat/res/zhiwu/1/%d.png", i + 1);
		loadimage(&g_img.xrk[i], buff);
	}
}
//透明图像绘制，winAPI
void transparentimagePNG(IMAGE* dstimg, int x, int y, IMAGE* srcimg, int alpha) {
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	//x -= w / 2;
	//y -= h / 2;
	// 结构体的第三个成员表示额外的透明度，0 表示全透明，255 表示不透明。
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
	// 使用 Windows GDI 函数实现半透明位图
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}
void putimg(int x,int y,IMAGE *img) {
	transparentimagePNG(0, x, y, img,255);
}
void putItem(_pair pos, int index, int clazz,int alpha) {
	int x = pos.x, y = pos.y;
	switch (clazz){
	case ZM:
		transparentimagePNG(0, x - g_img.zm[index].getwidth() / 2,
			y - g_img.zm[index].getheight() / 2,
			&g_img.zm[index], alpha);
		break;
	case WD:
		transparentimagePNG(0, x - g_img.wd[index].getwidth() / 4,
			y - g_img.wd[index].getheight() / 4,
			&g_img.wd[index], alpha);
		break;
	case XRK:
		transparentimagePNG(0, x - g_img.xrk[index].getwidth() / 4,
			y - g_img.xrk[index].getheight() / 4,
			&g_img.xrk[index], alpha);
		
		break;
	default:
		transparentimagePNG(0, x - g_img.xuanwo.getwidth() / 4,
			y - g_img.xuanwo.getheight() / 4,
			&g_img.xuanwo, alpha);
		fillcircle(x - g_img.xuanwo.getwidth() / 4, y - g_img.xuanwo.getheight() / 4, 5);
		break;
	}
}

