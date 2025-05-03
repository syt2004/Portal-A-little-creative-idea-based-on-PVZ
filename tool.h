#pragma once

#include "pch.h"
#include "uboo.h"
#include "list.h"

double distan(int x0,int y0,int x1,int y1) {
	return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}
double distan(_pair p0,_pair p1) {
	return sqrt(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2));
}
void drawLabel(_pair pos,_pair wh,const char *text,int Hight=30,COLORREF c=BLACK) {
	RECT rect;
	rect.left = pos.x;
	rect.top = pos.y;
	rect.right = pos.x + wh.x;
	rect.bottom = pos.y + wh.y;
	settextstyle(Hight, 0, "ËÎÌו");
	settextcolor(c);
	drawtext(text,&rect,DT_CENTER);
}
void drawLabeli(_pair pos, _pair wh, int s, int Hight = 30, COLORREF c = BLACK) {
	char text[64];
	RECT rect;
	rect.left = pos.x;
	rect.top = pos.y;
	rect.right = pos.x + wh.x;
	rect.bottom = pos.y + wh.y;
	settextstyle(Hight, 0, "ËÎÌו");
	settextcolor(c);
	sprintf(text, "%d", s);
	drawtext(text, &rect, DT_CENTER);
}
int isInRect(int x0,int y0,int x1,int y1,ExMessage msg) {
	return (x0 <= msg.x && msg.x <= x1) && (y0 <= msg.y && msg.y <= y1);
}
int isInRect_(int x0, int y0, int w, int h, ExMessage msg) {
	return (x0 <= msg.x && msg.x <= x0 + w) && (y0 <= msg.y && msg.y <= y0 + h);
}
int isInRect(_pair pos,_pair wh, ExMessage msg) {
	return (pos.x <= msg.x && msg.x <= pos.x + wh.x) && (pos.y <= msg.y && msg.y <= pos.y + wh.y);
}
