#pragma once
#include <stdint.h>

void setup_idt();
void setup_gdt();

void __cdecl printf(char* format, ...);

void outb(unsigned short, unsigned char);
void outw(unsigned short, unsigned short);
uint16_t inw(unsigned short);
uint8_t inb(unsigned short);

#define __VOID_SS		0x0
#define __KERNEL_CS	0x10	
#define __KERNEL_DS	0x18
#define __USER_CS		(0x20+3)
#define __USER_DS		(0x28+3)
#define __TSS			