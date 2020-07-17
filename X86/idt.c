#include "header.h"

#define __init

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
	255*8-1,
	(uint32_t)g_idt_table
};
#pragma pack(pop)


#define SAVE_ALL	\
__asm {push ebp} \
__asm { push ebx} \
__asm { push esi} \
__asm { push edi } \
__asm { push ecx } \
__asm { push edx } \
__asm { push eax } \
__asm { push fs } \
__asm { push ds } \
__asm { push es } 

#define RESTORE_ALL	\
__asm { pop es } \
__asm { pop ds } \
__asm { pop fs } \
__asm { pop eax } \
__asm { pop edx } \
__asm { pop ecx } \
__asm { pop edi } \
__asm { pop esi} \
__asm { pop ebx} \
__asm {pop ebp} 


/*
typedef struct _CALL_GATE
{
	USHORT OffsetLow;
	USHORT Selector;
	UCHAR NumberOfArguments:5;
	UCHAR Reserved:3;
	UCHAR Type:5;
	UCHAR Dpl:2;
	UCHAR Present:1;
	USHORT OffsetHigh;
}CALL_GATE,*PCALL_GATE;

typedef struct _INT_GATE
{
	USHORT OffsetLow;
	USHORT Selector;
	UCHAR NumberOfArguments:5;
	UCHAR Reserved:3;
	UCHAR Type:5;
	UCHAR Dpl:2;
	UCHAR Present:1;
	USHORT OffsetHigh;
}INT_GATE,*INT_GATE;
*/
static void _set_gate(void *p, unsigned type, unsigned DPL, void *handler)
{
	X86IDT *entry = (X86IDT *)p;
	entry->high = ((unsigned)handler & 0xffff0000) | 0x8000 | (DPL<<13) | (type <<8);
	entry->low =  ((unsigned)handler & 0x0000ffff) | (0x10 << 16);
	
	//0x8000=P
	
}

//difference between 
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

}

void debug()
{
}

void nmi()
{
}

uint32_t page_fault_addr = 0;
__declspec (naked) void page_fault()
{
	SAVE_ALL
	__asm {
		mov eax, cr2
		mov page_fault_addr, eax
	}
	printf("page fault @%x\n", page_fault_addr);
	RESTORE_ALL
	__asm {
		add esp, 4	//skip error_code
		add [esp], 0xA	//skip	instruction trigger it
		iretd
	}
}

__declspec (naked) void double_fault()
{
	printf("double fault\n");
}

__declspec (naked) void int3()
{
	SAVE_ALL
	printf("int 3\n");
	RESTORE_ALL
	__asm {
		iretd	//̫�ӡ� iret 0x66CF �� iretd 0xCF ���ǲ�һ����
		iret
	}
}

__declspec (naked) void general_protection()
{
	
	__asm {
		mov ax, 0x18
		mov ds, ax;			//fix
	}
	SAVE_ALL
	printf("general_protection\n");
	RESTORE_ALL
	__asm {
		iretd	//̫�ӡ� iret �� iretd ���ǲ�һ����
		iret
	}
}

void kbdirq()
{
	
}

void clockirq()
{
	printf("clock\n");
}
	
#define HZ 100
#define CLOCK_TICK_RATE    1193180    /* ͼ5.3�е��������� */
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)  /* ������0�ļ�����ֵ */

void init_clock()
{
	outb(0x34,0x43);     				/* д������0�Ŀ����֣�������ʽ2*/
 	outb(LATCH & 0xff , 0x40); 		  /* д������ֵLSB  ������ֵ��λ�ֽ�*/  
	outb(LATCH >> 8 , 0x40);  		 /* д������ֵMSB ������ֵ��λ�ֽ�*/
}


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


struct pt_regs {
	long ebx;
	long ecx;
	long edx;
	long esi;
	long edi;
	long ebp;
	long eax;
	int  xds;
	int  xes;
	//---------------------------
	long orig_eax;
	long eip;
	int  xcs;
	long eflags;
	long esp;
	int  xss;
};

void setup_idt_and_test()
{
	set_trap_gate(0, &divide_error);
	set_system_gate(3, &int3);
	set_trap_gate(8,	&double_fault);
	set_trap_gate(13,&general_protection);
	set_trap_gate(14, &page_fault);
	__asm {
		lidt idt_desc
	}
	
	//test
	__asm {
		int 3	//test int 3
	}
	int *pagefault = (int *) 0xf000A000;
	*pagefault = 0x233;	
	
	/*
	__asm {
		mov ax, 0x8
		mov ds, ax
		mov eax, 0xf000A000		//����μ�ⲻ������ᴥ�� GP#13
		mov [eax], eax
	}
	*/
	return;
}

void setup_idt()
{
	set_trap_gate(0, &divide_error);
	set_trap_gate(1, &debug);
	set_intr_gate(2, &nmi);
	set_system_gate(3, &int3);

	set_trap_gate(8,	&double_fault);
	
	set_trap_gate(13,&general_protection);
	set_trap_gate(14, &page_fault);
	
	set_intr_gate(32, &clockirq);
	set_intr_gate(33, &kbdirq);
	
	__asm {
		lidt idt_desc
	}
	return;
}