#include "list.h"
void* memcp(void* ptr, int size) {
	void* p = mem(void, size);
	memcpy(p, ptr, size);
	return p;
}
List ListInit() {
	List p = mem(Node, sizeof(Node));
	memset(p, 0, sizeof(Node));
	p->dat.num = 0;
	p->next = p;
	p->prev = p;
	return p;
}
int ListSize(List plist) {
	return plist->dat.num;
}
void* ListIndex(List plist, int i) {
	List p = plist->next;
	for (; i--;p=p->next);
	return p->dat.p;
}
void ListInsert(List plist,void* dat,int i) {
	List p = plist->next,ptr=mem(Node,sizeof(Node));
	for (; i--; p = p->next);
	//p = p->prev;
	ptr->dat.p = dat;
	ptr->next = p;
	ptr->prev = p->prev;
	ptr->prev->next = ptr;
	ptr->next->prev = ptr;
	plist->dat.num++;
}
void ListDelete(List plist, int i) {
	List p = plist->next;
	for (; i--; p = p->next);
	free(p->dat.p);
	p->next->prev = p->prev;
	p->prev->next = p->next;
	plist->dat.num--;
	free(p);
}
void ListPushback(List plist, void* dat) {
	List p = plist->prev, ptr = mem(Node, sizeof(Node));
	ptr->next = plist;
	ptr->prev = p;
	p->next = ptr;
	plist->prev = ptr;
	ptr->dat.p = dat;
	plist->dat.num++;
}
void ListPopback(List plist) {
	List p = plist->prev;
	p->next->prev = p->prev;
	p->prev->next = p->next;
	free(p->dat.p);
	free(p);
	plist->dat.num--;
}
void ListPushbacki(List plist, void* dat, int size) {
	ListPushback(plist, memcp(dat, size));
}