#include <stdint.h>

#define __init

typedef struct TSS {
    uint32_t link; // ����ǰһ�� TSS ��ѡ���ӣ�ʹ�� call ָ���л��Ĵ�����ʱ����CPU��д��
    // �� 6 ��ֵ�ǹ̶�����ģ�������Ȩ��CPU �л�ջ��ʱ����
    uint32_t esp0; // ���� 0 ��ջָ��
    uint32_t ss0;  // ���� 0 ��ջ��ѡ����
    uint32_t esp1; // ���� 1 ��ջָ��
    uint32_t ss1;  // ���� 1 ��ջ��ѡ����
    uint32_t esp2; // ���� 2 ��ջָ��
    uint32_t ss2;  // ���� 2 ��ջ��ѡ����
    // ������Щ�����������л��Ĵ���ֵ�õģ��л��Ĵ�����ʱ����CPU�Զ���д��
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
    // �����ʱ����
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

GDT_ENTRY g_gdt_entries[10] = {0};

uint16_t		lgdt_limit = 10;
uint32_t		lgdt_entry = (uint32_t)g_gdt_entries;


static inline void set_gdt(void *e, void *base, int dpl)
{
	unsigned addr = (unsigned)base;
	GDT_ENTRY *entry = (GDT_ENTRY *)e;
	entry->limit15 = 0xffff;
	entry->base15 = addr & 0xffff;
	entry->base23 = (addr >> 16) & 0xff;
	entry->base31 = (addr >> 24) & 0xff;
	entry->PDPLSType = 0xC0;
	entry->GDBAlimit19 = 0xCF;
	return;
}


void setup_gdt()
{
	set_gdt(g_gdt_entries + 1, 0, 3);
	set_gdt(g_gdt_entries + 2, 0, 3);
	set_gdt(g_gdt_entries + 3, 0, 0);
	set_gdt(g_gdt_entries + 4, 0, 0);
	
	g_tss.esp0 = 0x300000;
	set_gdt(g_gdt_entries + 5, &g_tss, 0);
	
	__asm {
		lgdt 	lgdt_limit
		mov	ax, 0x10
		mov	ds, ax
		mov	es, ax
		mov	ss, ax
	}
	return;
}
