
#include <stdint.h>

void outb(unsigned short, unsigned char);
void outw(unsigned short, unsigned short);
uint16_t inw(unsigned short);
uint8_t inb(unsigned short);

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