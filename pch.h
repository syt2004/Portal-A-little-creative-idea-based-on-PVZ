#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <graphics.h>
#include <conio.h>
#include <stdarg.h>
#include <Windows.h>

#pragma warning(disable:4996)
#pragma comment(lib,"Winmm.lib")
#pragma comment( lib, "MSIMG32.LIB")

#define cast(t,x) ((t)x)//强制类型转换
#define array(t,x) *((t*)x)//强制类型数组转换
#define LOG(tag,x,...) printf("[%s]:",tag),printf(x,__VA_ARGS__),printf("\r\n")//自定义日志输出
#define LOGI(x,...) LOG("info",x,...)//日志输出

#define W 1000
#define H 800

#define ZM_NUM 22
#define WD_NUM 13
#define XRK_NUM 18

#define ZM 0
#define WD 1
#define XRK 2

typedef struct Node{
	union {
		void* p;
		int num;
	}dat;
	struct Node* prev, * next;
}Node;
typedef struct _pair{
	int x, y;
}_pair;

typedef struct {
	_pair pos;
	int HP;
	int imgid;
	clock_t pre;//生成间隔
}Plant;
typedef struct {
	_pair pos;
	int HP;
	int CanMove;
	int index;
	clock_t pre;
}zb;
typedef struct {
	_pair pos;
	clock_t pre;
}sunshine;

void mainGame();
