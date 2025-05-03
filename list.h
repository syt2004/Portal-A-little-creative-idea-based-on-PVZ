#pragma once
#include "pch.h"

#define mem(t,x) cast(t*,malloc(x))

typedef Node* List;

void* memcp(void *ptr,int size);
List ListInit();
int ListSize(List);
void* ListIndex(List, int);
void ListInsert(List, void*, int);
void ListDelete(List, int);
void ListPushback(List,void *);
void ListPopback(List);
void ListPushbacki(List,void *,int);
//void ListPopbacki();

