#include "pch.h"
#include "uboo.h"

int main() {
	initgraph(W,H,SHOWCONSOLE);
	setbkcolor(BLUE);
	setbkmode(TRANSPARENT);
	LoadimageDat();
	mainGame();
	closegraph();
	return 0;
}