#pragma once

#include "pch.h"
#include "vector"
#include "string"
#include "fstream"

#define QWORD unsigned long long
#define LZWBIGGESTBITSIZE 	12

using namespace std;

#pragma pack(1)// 强制 1byte 对齐

struct gifinf
{
	WORD x, y;				// 图片长宽
	BYTE MCSP;				// 杂项（包括是否拥有gct，颜色深度，分类标识，gct大小）
	BYTE backgroundcolor;	// 背景色
	BYTE pixelwh;
	DWORD* gcolortab;		// 全局颜色表
};

#pragma pack(1)				// 强制 1byte 对齐

struct gifgce				// 图形控制拓展
{
	BYTE HUT;				// 杂项（包括处置方法，用户输入标识，透明色标识）
	WORD sleep;				// 延迟时间（百分之一秒）
	BYTE transparent;		// 透明色索引
};
struct gifidescriptor
{
	WORD xoffset, yoffset;	// x、y方向偏移
	WORD width, height;		// 宽高
	BYTE MISRP;				// 杂项（包括是否有lct，交织|顺序排列，分类标志，lct大小）
	DWORD* lcolortab;		// 局部颜色表 
};
struct frame
{
	BYTE* data;				// 帧的信息（解压后的）
	gifgce gce;
	gifidescriptor gid;
};

class gifimage//注：loadgif加载gif时，加载分辨率过高或时间过长的gif会耗时较多（详见代码末尾的速度测试）
{
private:
	vector<frame> frms;		// 所有帧
	DWORD framep;			// 显示到第几帧
	gifinf info;			// gif 的信息
	// 从内存中读取一个数据 
	template<typename m>
	void readnuminmemory(void* buf, m& to)
	{
		to = *((m*)buf);
		return;
	}
	// 获取文件大小
	QWORD getfilesize(const char* file)
	{
		QWORD ret = 0;
		ifstream fin(file, ifstream::ate | ifstream::binary);
		ret = fin.tellg();
		fin.close();
		return ret;
	}
	// 将时间转换为毫秒
	DWORD tsltime()
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		return time.wDay * 24 * 60 * 60 * 1000 + time.wHour * 60 * 60 * 1000 + \
			time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;
	}
	// 获取gif的基本信息
	void getinfo(BYTE* filebuf, gifinf& info, DWORD& p)
	{
		readnuminmemory(filebuf + 6, info);

		p = 13;						// 全局颜色表起始点
		if (info.MCSP & (1 << 7))	// 有全局颜色表
		{
			DWORD gctsize = (DWORD)pow((double)2, (info.MCSP & 7) + 1);
			info.gcolortab = new DWORD[gctsize];
			for (DWORD i = 0; i < gctsize; i++, p += 3)
				info.gcolortab[i] = RGB(filebuf[p], filebuf[p + 1], filebuf[p + 2]);
		}

		WORD nc20;
		readnuminmemory(filebuf + p, nc20);
		if (nc20 == 0xff21)
			p += 2 + 17;// 跳过 netscape2.0 等信息
		return;
	}
	// 解码一帧画面，如果到文件的末尾就返回 0
	int decodeframe(frame& ret, BYTE* filebuf, gifinf info, DWORD& p)
	{
		if (filebuf[p] == 0x3B)	// 文件结尾
			return 0;
		p += 3;					// 跳过拓展快表示、扩展标签、块大小，因为都是定值

		readnuminmemory(filebuf + p, ret.gce);
		p += 4;

		p++;// 跳过块终止符

		WORD asciitext = 0;
		readnuminmemory(filebuf + p, asciitext);

		if (asciitext != 0xfe21)
			goto skipasciiover;
		p += 2;							// 跳过 ASCII 文本嵌入块的头
		for (; filebuf[p] != 0; p++);	// 跳过 ASCII 文本
		p++;// 跳过 \0
	skipasciiover:

		p++;// 跳过图像标识符

		readnuminmemory(filebuf + p, ret.gid);// 获取帧信息
		p += 9;

		if (ret.gid.MISRP & (1 << 6))
			return 0;

		if (ret.gid.MISRP & (1 << 7))// 是否拥有局部颜色表
		{
			DWORD lctsize = (DWORD)pow((double)2, (ret.gid.MISRP & 7) + 1);
			ret.gid.lcolortab = new DWORD[lctsize];
			for (DWORD i = 0; i < lctsize; i++, p += 3)
				ret.gid.lcolortab[i] = RGB(filebuf[p], filebuf[p + 1], filebuf[p + 2]);
		}

		BYTE lzwts = filebuf[p++];
		DWORD bufsize = 0, tbp = p;
		while (filebuf[tbp])// 获取帧数据大小
		{
			bufsize += filebuf[tbp];
			tbp += filebuf[tbp] + 1;
		}

		DWORD lzp = 0;
		BYTE* lzwbuf = new BYTE[bufsize];
		while (filebuf[p])// 取得帧数据
		{
			memcpy(lzwbuf + lzp, filebuf + p + 1, filebuf[p]);
			lzp += filebuf[p];
			p += filebuf[p] + 1;
		}

		p++;// 跳过 \0
		vector<BYTE> a = lzwdecompress(lzwbuf, lzp, lzwts);// 解压缩并将处理后的数据放进帧的结构体

		ret.data = new BYTE[a.size()];
		for (DWORD i = 0; i < a.size(); i++)
			ret.data[i] = a[i];

		if (filebuf[p] == 0x21 && filebuf[p + 1] == 0xff && filebuf[p + 2] == 0x0b)// gif 中与解码无关的部分 (application extension)
		{
			p += 3;
			while (filebuf[p++] != 0);
		}
		if (filebuf[p] == 0x21 && filebuf[p + 1] == 1)// gif 中中与解码无关的部分*2 (plain text extension)
		{
			p += 2;
			p += filebuf[p] + 1;
			while (filebuf[p++] != 0);
		}

		delete[] lzwbuf;
		return 1;
	}
	// lzw 解压，读取指定数目的bit位
	DWORD lzwbitread(BYTE* buf, DWORD& bufp, BYTE& read, DWORD& bitp, DWORD needbit)
	{
		DWORD ret = 0, ti = needbit;
		while (needbit)
		{
			ret += (read & 1) << (ti - needbit);
			read >>= 1;
			bitp++;
			if (bitp == 8)
			{
				read = buf[bufp++];
				bitp = 0;
			}
			needbit--;
		}
		return ret;
	}
	// lzw 算法解压
	vector<BYTE> lzwdecompress(BYTE* lzwbuf, DWORD lzwsz, DWORD stws)
	{
		vector<BYTE> outbuf;
		outbuf.reserve(20 * 1024 * 1024);//20 KB 预备

		DWORD cw, ps = (1 << stws) + 2, pw, bufp = 0, bitw = stws + 1, tp = 0;
		string* dictionary = new string[1 << (LZWBIGGESTBITSIZE + 1)];

		for (int i = 0; i < (1 << stws) + 2; i++)
			dictionary[i] = (string)"" + (char)i;
		BYTE read = lzwbuf[bufp++];

		while (bufp < lzwsz)
		{
			pw = lzwbitread(lzwbuf, bufp, read, tp, bitw);
		wl:
			if (bufp >= lzwsz)
				break;
			if (pw == (1 << stws))// clean code ，接收到此信息之后应该清空
			{
				ps = (1 << stws) + 2;
				bitw = stws + 1;
				continue;
			}

			if (pw >= ps)
				continue;
			if (pw == (1 << stws) + 1)
				break;

			for (DWORD j = 0; j < dictionary[pw].size(); j++)
				outbuf.push_back(dictionary[pw][j]);
			if (ps + 1 > (DWORD)(1 << bitw) && bitw < LZWBIGGESTBITSIZE)// GIF 压缩时 bitw 小于等于 12 
				bitw++;

			cw = lzwbitread(lzwbuf, bufp, read, tp, bitw);

			dictionary[ps++] = dictionary[pw] + dictionary[(cw < ps) ? cw : pw][0];
			pw = cw;

			goto wl;
		}
		delete[] dictionary;
		return outbuf;
	}
public:
	gifimage(DWORD frmnum)// 通过此构造函数预留空间，防止 vector 频繁 reserve 降低速度
	{
		frms.reserve(frmnum);// 预留帧数 
		framep = 0;
	}
	gifimage()
	{
		//frms.reserve(256);// 以256帧预留初始化 禁用预留
		framep = 0;
	}
	gifimage(const char* filename)
	{
		//frms.reserve(256);// 以256帧预留初始化 禁用预留
		framep = 0;
		loadgif(filename);
	}
	~gifimage()
	{
		for (int i = 0; i < (int)frms.size(); i++)
		{
			delete[] frms[i].data;
			if ((frms[i].gid.MISRP & (1 << 7)))
				delete[] frms[i].gid.lcolortab;
		}
		delete[] info.gcolortab;
	}
	//加载一个 gif 到对象（成功：true，失败：false）
	bool loadgif(const char* filename)
	{
		FILE* rd;
		fopen_s(&rd, filename, "rb");
		if (rd == NULL)
			return false;

		DWORD filesize = (DWORD)getfilesize(filename), filep = 0;// filesize:文件大小，filep：文件指针

		BYTE* buf = new BYTE[filesize];
		fread(buf, 1, filesize, rd);
		fclose(rd);

		if (strncmp((char*)buf, "GIF89a", 6) != 0)// 以下判断格式并获取信息
			return false;
		getinfo(buf, info, filep);

		frame tmp;
		while (decodeframe(tmp, buf, info, filep))
			frms.push_back(tmp);

		framep = 0;
		delete[] buf;
		return true;
	}
	//显示gif的下一帧(返回值表示当前帧等待的毫秒数，若返回 0xffffffff 则表示播放完毕，再次执行将重新播放）
	DWORD DisplayFrame(int ofx, int ofy)// ofx、ofy :显示偏移 
	{
		if (framep == frms.size())
		{
			framep = 0;
			return 0xffffffff;
		}
		BeginBatchDraw();// 开始批量绘制，这样的话可以防止闪屏
		frame frm = frms[framep++];
		if ((frm.gce.HUT & (0x1c)) == 2)// 使用背景色填充
			for (int i = 0; i < info.x; i++)
				for (int j = 0; j < info.y; j++)
					putpixel(i + ofx, j + ofy, info.gcolortab[info.backgroundcolor]);

		int ps = 0;
		for (int j = 0; j < frm.gid.height; j++)
			for (int i = 0; i < frm.gid.width; i++, ps++)
				if (frm.data[ps] != frm.gce.transparent)// 透明色
					putpixel(i + ofx + frm.gid.xoffset, j + ofy + frm.gid.yoffset, (frm.gid.MISRP & (1 << 7)) ? \
						frm.gid.lcolortab[frm.data[ps]] : info.gcolortab[frm.data[ps]]);
		EndBatchDraw();
		return frm.gce.sleep * 10;
	}
	//显示gif的下一帧(返回值表示当前帧等待的毫秒数，若返回 0xffffffff 则表示播放完毕，再次执行将重新播放）
	DWORD DisplayFrameNorm(int ofx, int ofy,int index)// ofx、ofy :显示偏移 
	{
		if (index >= frms.size()){
			//framep = 0;
			return 0xFF;
		}
		//BeginBatchDraw();// 开始批量绘制，这样的话可以防止闪屏
		frame frm = frms[index];
		if ((frm.gce.HUT & (0x1c)) == 2)// 使用背景色填充
			for (int i = 0; i < info.x; i++)
				for (int j = 0; j < info.y; j++)
					putpixel(i + ofx, j + ofy, info.gcolortab[info.backgroundcolor]);

		int ps = 0;
		for (int j = 0; j < frm.gid.height; j++)
			for (int i = 0; i < frm.gid.width; i++, ps++)
				if (frm.data[ps] != frm.gce.transparent)// 透明色
					putpixel(i + ofx + frm.gid.xoffset, j + ofy + frm.gid.yoffset, (frm.gid.MISRP & (1 << 7)) ? \
						frm.gid.lcolortab[frm.data[ps]] : info.gcolortab[frm.data[ps]]);
		//EndBatchDraw();
		return 0;
	}
	//播放整个 gif 直到播放完毕
	void DisplayGif(int ofx, int ofy)
	{
		DWORD starttime = (DWORD)tsltime(), sleep;
		while ((sleep = DisplayFrame(ofx, ofy)) != 0xffffffff)
		{
			while (tsltime() < starttime + sleep)// 等待到下一帧播放的时间
				Sleep(1);
			starttime += sleep;
		}
		return;
	}
	//设置帧指针（即：设置 gif 视频播放进度）
	void SetFramePointer(DWORD set)
	{
		framep = set;
	}
	//获取帧指针（即：获取 gif 播放进度）
	DWORD GetFramePointer()
	{
		return framep;
	}
	//获取帧的缓冲区 
	vector<frame>& GetFrameBuffer()
	{
		return frms;
	}
	gifinf GetGifInfo()
	{
		return info;
	}
	WORD GetWidth()
	{
		return info.x;
	}
	WORD GetHeight()
	{
		return info.y;
	}
	//重载赋值运算
	gifimage operator = (gifimage equ)
	{
		frms = equ.GetFrameBuffer();
		framep = equ.GetFramePointer();
		info = equ.GetGifInfo();
		return *this;
	}
};