#pragma once
#include "pch.h"
#include "gif.hpp"
typedef struct {
	IMAGE Map,Card_xrk,Card_wd,Card_h,Sunshine;
	IMAGE zm[ZM_NUM], bar, wd[WD_NUM],xrk[XRK_NUM];
	IMAGE xuanwo;
}Img;
extern Img g_img;

void testPos();
void LoadimageDat();
void putimg(int x, int y, IMAGE* img);
void putItem(_pair pos, int index, int clazz, int alpha=255);
