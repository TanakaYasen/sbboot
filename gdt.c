#include "header.h"

#define __init

typedef struct TSS {
    uint32_t link; // 保存前一个 TSS 段选择子，使用 call 指令切换寄存器的时候由CPU填写。
    // 这 6 个值是固定不变的，用于提权，CPU 切换栈的时候用
    uint32_t esp0; // 保存 0 环栈指针
    uint32_t ss0;  // 保存 0 环栈段选择子
    uint32_t esp1; // 保存 1 环栈指针
    uint32_t ss1;  // 保存 1 环栈段选择子
    uint32_t esp2; // 保存 2 环栈指针
    uint32_t ss2;  // 保存 2 环栈段选择子
    // 下面这些都是用来做切换寄存器值用的，切换寄存器的时候由CPU自动填写。
    uint32_t cr3; 
    uint32_t eip;  
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    // 这个暂时忽略
    uint32_t io_map;
} TSS;

/*
typedef struct {
    unsigned  int  base24_31:8; 
    unsigned  int  g:1;
    unsigned  int  d_b:1;
    unsigned  int  unused:1;
    unsigned  int  avl:1;
    unsigned  int  seg_limit_16_19:4;
    unsigned  int  p:1;
    unsigned  int  dpl:2;
    unsigned  int  s:1;
    unsigned  int  type:4;
    unsigned  int  base_0_23:24;
    unsigned  int  seg_limit_0_15:16; 
}
*/
   
//https://en.wikipedia.org/wiki/Global_Descriptor_Table
typedef struct GDT_ENTRY {
	uint16_t		limit15;
	uint16_t		base15;
	uint8_t		base23;
	uint8_t		PDPLSType;
	uint8_t		GDBAlimit19;
	uint8_t		base31;
}GDT_ENTRY;

TSS g_tss;

GDT_ENTRY g_gdt_entries[7] = {0};


#pragma pack(push)
#pragma pack(2)
struct X86_GDT_DESC
{
	uint16_t		len;
	uint32_t		offset;
}  gdt_desc = {
	7*sizeof(GDT_ENTRY)-1,
	(uint32_t)g_gdt_entries
};
#pragma pack(pop)



static inline void set_gdt(void *e, void *base, int type, int dpl)
{
	//TYPE = E + ED/C + RW
	// E=0:data segment	ED=0:UP(data seg), ED=1:DN(stack seg)	W=0:Writeless W=1:Writable
	// E=0:code segment	C=0:ignore priv C=1:obey priv	R=0: Readless R=1: Readable
	
	unsigned addr = (unsigned)base;
	GDT_ENTRY *entry = (GDT_ENTRY *)e;
	entry->limit15 = 0xffff;
	entry->base15 = addr & 0xffff;
	entry->base23 = (addr >> 16) & 0xff;
	entry->base31 = (addr >> 24) & 0xff;
	entry->PDPLSType = 0x90 | (dpl << 5) | type;
	entry->GDBAlimit19 = 0xCF;
	return;
}


void setup_gdt()
{
	set_gdt(g_gdt_entries + 1, 0, 0xA, 0);
	set_gdt(g_gdt_entries + 2, 0, 0x2, 0);
	set_gdt(g_gdt_entries + 3, 0, 0xA, 3);
	set_gdt(g_gdt_entries + 4, 0, 0x2, 3);
	
	g_tss.esp0 = 0x300000;
	set_gdt(g_gdt_entries + 5, &g_tss, 6, 0);
	
	__asm {
		lgdt 	gdt_desc
		mov	ax, __KERNEL_DS
		mov	ds, ax
		mov	es, ax
		mov	ss, ax
		push 	__KERNEL_CS
		push	__switch
		retf
__switch:
		
	}
	return;
}
