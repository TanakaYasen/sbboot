
#include "header.h"

//floppy		https://blog.csdn.net/judyge/article/details/52289104

//harddisk		https://blog.csdn.net/fjlq1994/article/details/49472827

// mouse		

// keyboard		https://www.jianshu.com/p/b45b36261274

// vga			https://blog.csdn.net/qq_39148922/article/details/85005271

int ReadDisk(unsigned lba, unsigned nsec, char *p, int limit)
{
	outb(0x1f2, (unsigned char)nsec);			//nsec
	
	outb(0x1f3, (unsigned char)(lba & 0xff));		//lba	0~7
	outb(0x1f4, (unsigned char)((lba>>8)&0xff));	//lba	8~15
	outb(0x1f5, (unsigned char)((lba>>6)&0xff));	//lba	16~23
	outb(0x1f6, (unsigned char)((lba>>24)&0xf));	//lba	24~27
	//
	outb(0x1f7, 0x20);
	unsigned char b = inb(0x1f7);
	
	do
	{
		b = inw(0x1f7) &0x88;
	}
	while (b != 0x8);	// wait
	
	unsigned short tmp = inw(0x1f0);
	unsigned short *w = (unsigned short *)p;
	while (limit > 0)
	{
		*w++ = tmp;
		limit -= 2;
	}
	return limit;
}