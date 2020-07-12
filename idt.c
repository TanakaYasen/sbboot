#include "header.h"

void __cdecl printf(char* format, ...);

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
	255,
	(uint32_t)g_idt_table
};
#pragma pack(pop)

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
	entry->low =  ((unsigned)handler & 0x0000ffff) | (__KERNEL_CS << 16);
	
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
	__asm {
		nop
			nop
			nop
		push ebp  
		push ebx  
		push esi  
		push edi  
		push fs  
		push ecx  
		push edx  
		push ds  
		push es
		push eax
		mov eax, cr2
		mov page_fault_addr, eax
	}
	printf("page fault @%x\n", page_fault_addr);
	__asm {
		pop eax
		pop es
		pop ds  
		pop edx  
		pop ecx  
		pop fs  
		pop edi  
		pop esi  
		pop ebx  
		pop ebp  
		add esp, 4	//skip error_code
		add [esp], 0xA	//skip	instruction trigger it
		iretd	//太坑。 iret 0x66CF 与 iretd 0xCF 还是不一样的
		iret
	}
}

__declspec (naked) void double_fault()
{
	__asm {
		nop
		nop
		nop
		xor eax, eax
	}
	printf("double fault\n");
}

__declspec (naked) void int3()
{
	__asm {
		push ebp  
		push ebx  
		push esi  
		push edi  
		push fs  
		push ecx  
		push edx  
		push ds  
		push es
		push eax
	}
	printf("int 3\n");
	__asm {
		pop eax
		pop es
		pop ds  
		pop edx  
		pop ecx  
		pop fs  
		pop edi  
		pop esi  
		pop ebx  
		pop ebp  
			
		iretd	//太坑。 iret 与 iretd 还是不一样的
		iret
	}
}

__declspec (naked) void general_protection()
{
	
	__asm {
		mov ax, 0x18
		mov ds, ax;			//fix
		push ebp  
		push ebx  
		push esi  
		push edi  
		push fs  
		push ecx  
		push edx  
		push ds  
		push es
		push eax
	}
	printf("general_protection\n");
	__asm {
		pop eax
		pop es
		pop ds  
		pop edx  
		pop ecx  
		pop fs  
		pop edi  
		pop esi  
		pop ebx  
		pop ebp  
			
		iretd	//太坑。 iret 与 iretd 还是不一样的
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
#define CLOCK_TICK_RATE    1193180    /* 图5.3中的输入脉冲 */
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)  /* 计数器0的计数初值 */

void init_clock()
{
	outb(0x34,0x43);     				/* 写计数器0的控制字：工作方式2*/
 	outb(LATCH & 0xff , 0x40); 		  /* 写计数初值LSB  计数初值低位字节*/  
	outb(LATCH >> 8 , 0x40);  		 /* 写计数初值MSB 计数初值高位字节*/
}


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
	
	init_clock();
	return;
}

int test_idt(int y)
{
	__asm {
		int 3	//test int 3
	}
	int *pagefault = (int *) 0xf000A000;
	*pagefault = 0x233;	
	
	__asm {
		mov ax, 0x8
		mov ds, ax
		mov eax, 0xf000A000		//如果段检测不过，则会触发 GP#13
		mov [eax], eax
	}
	
	int x = 3;
	x /= y;
	return x;
}