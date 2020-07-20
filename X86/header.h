#pragma once
#include <stdint.h>

void setup_idt_and_test();
void setup_gdt();
void setup_idt();
void demo_graphics(void);

void __cdecl printf(const char* format, ...);
void vesa_information();

void outb(unsigned short, unsigned char);
void outw(unsigned short, unsigned short);
uint16_t inw(unsigned short);
uint8_t inb(unsigned short);

#define __VOID_SS		0x0
#define __KERNEL_CS	0x8
#define __KERNEL_DS	0x10
#define __USER_CS		(0x18+3)
#define __USER_DS		(0x20+3)
#define __TSS			