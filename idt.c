#include <stdint.h>

#define __init
#define __KERNEL_CS 0x10

typedef struct _X86IDT {
	uint32_t		low;
	uint32_t		high;
} X86IDT;

X86IDT	 g_idt_table[256];

#pragma pack(push)
#pragma pack(2)
struct X86_IDT_DESC
{
	uint16_t		len;
	uint32_t		offset;
}  idt_desc = {
	255,
	(uint32_t)g_idt_table
};
#pragma pack(pop)

static void _set_gate(void *p, unsigned type, unsigned DPL, void *handler)
{
	X86IDT *entry = (X86IDT *)p;
	entry->high = ((unsigned)handler & 0xffff0000) | 0x8000 | (DPL<<13) | (type <<8);
	entry->low =  ((unsigned)handler & 0x0000ffff) | (__KERNEL_CS << 16);
	
	//0x8000=P
	
}

static void __init set_intr_gate(unsigned int n, void *addr)
{
	_set_gate(g_idt_table+n,14,0,addr);
}

static void __init set_trap_gate(unsigned int n, void *addr)
{
	_set_gate(g_idt_table+n,15,0,addr);
}

static void __init set_system_gate(unsigned int n, void *addr)
{
	_set_gate(g_idt_table+n,15,3,addr);
}

static void __init set_call_gate(void *a, void *addr)
{
	_set_gate(g_idt_table,12,3,addr);
}

void divide_error()
{
	__asm {
		nop
		nop
		nop
	}
}

void debug()
{
	__asm {
		nop
		nop
		nop
		nop
		nop
	}
}

void nmi()
{
	__asm {
		nop
		nop
		int 3
		int 4
		nop
	}
}

void page_fault()
{
	__asm {
		mov eax, cr2
		nop
	}
}

const char int3msg[]="int3"; 
void int3()
{
	unsigned short *p =(unsigned short *)0xb8000;
	for (int i = 0; i < sizeof(int3msg); i++)
	{
		*p++ = 0xd00 | int3msg[i];
	}
}

void kbdirq()
{
}

void clockirq()
{
}
	
void setup_idt()
{
	set_trap_gate(0, &divide_error);
	set_trap_gate(1, &debug);
	set_intr_gate(2, &nmi);
	set_system_gate(3, &int3);

	set_trap_gate(14, &page_fault);
	
	set_intr_gate(32, &clockirq);
	set_intr_gate(33, &kbdirq);
	
	__asm {
		lidt idt_desc
		int 3
	}
	
	
	
}


