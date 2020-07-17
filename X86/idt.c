#include "header.h"

#define __init

#define _OLD_KERNRL_CS			0x10

#define _MASTE_PIC_COMMAND	0x20
#define _MASTE_PIC_DATA			0x21
#define _SLAVE_PIC_COMMAND		0xA0
#define _SLAVE_PIC_DATA			0xA1

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

static unsigned short	__IDT_CS__;

static void _set_gate(void *p, unsigned type, unsigned DPL, void *handler)
{
	X86IDT *entry = (X86IDT *)p;
	entry->high = ((unsigned)handler & 0xffff0000) | 0x8000 | (DPL<<13) | (type <<8);
	entry->low =  ((unsigned)handler & 0x0000ffff) | (__IDT_CS__ << 16);
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


uint32_t page_fault_addr = 0;
__declspec (naked) void _test_page_fault()
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

__declspec (naked) void _test_int3()
{
	SAVE_ALL
	printf("int 3\n");
	RESTORE_ALL
	__asm {
		iretd	//太坑。 iret 0x66CF 与 iretd 0xCF 还是不一样的
		iret
	}
}

__declspec (naked) void _test_general_protection()
{
	__asm {
		mov ax, 0x18
		mov ds, ax;			//fix
	}
	SAVE_ALL
	printf("general_protection\n");
	RESTORE_ALL
	__asm {
		iretd	//太坑。 iret 与 iretd 还是不一样的
		iret
	}
}

void setup_idt_and_test()
{
	__IDT_CS__ = _OLD_KERNRL_CS;
	set_system_gate(3, 	&_test_int3);
	set_trap_gate(13,	&_test_general_protection);
	set_trap_gate(14, 	&_test_page_fault);
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
		mov eax, 0xf000A000		//如果段检测不过，则会触发 GP#13
		mov [eax], eax
	}
	*/
	return;
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
	SAVE_ALL
	printf("double fault\n");
	RESTORE_ALL
	__asm {
		add esp, 4	//skip error_code
		iretd
	}
}

__declspec (naked) void int3()
{
	SAVE_ALL
	printf("int 3\n");
	RESTORE_ALL
	__asm {
		iretd	//太坑。 iret 0x66CF 与 iretd 0xCF 还是不一样的
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
		iretd
	}
}

__declspec (naked) void clockirq()
{
	static int clock_count;
	SAVE_ALL
	//printf("clock irq %d\n", clock_count++);
	//__asm {sti}	; 	//开中断
	//while (1);		//再循环才能被中断
	RESTORE_ALL
	__asm {
		sti			//not need.
		iretd		//this would recover eflag and thus, IF will be reset.
	}
}

__declspec (naked) void kbdirq()
{
	SAVE_ALL
	printf("kbdirq %d\n", inb(0x60));
	RESTORE_ALL
	__asm {
		sti
		iretd
	}
}

__declspec (naked) void common_irq()
{
	static int irq_count;
	SAVE_ALL
	printf("common irq %d\n", irq_count++);
	RESTORE_ALL
	__asm {
		sti
		iretd
	}
}

inline void send_eoi(void)
{
     /* Send EOI to both master and slave */
    outb( 0x20, 0x20 ); /* master PIC */
    outb( 0x20, 0xA0 ); /* slave PIC */
}

#define HZ 100
#define CLOCK_TICK_RATE    1193180    /* 图5.3中的输入脉冲 */
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)  /* 计数器0的计数初值 */

void init_clock()
{
	outb(0x34, 0x43);     				/* 写计数器0的控制字：工作方式2*/
 	outb(LATCH & 0xff , 0x40); 		  /* 写计数初值LSB  计数初值低位字节*/  
	outb(LATCH >> 8 , 0x40);  		 /* 写计数初值MSB 计数初值高位字节*/
}

//https://wiki.osdev.org/8259_PIC
//https://docs.huihoo.com/gnu_linux/own_os/interrupt-8259_5.htm


void init_8259_pic()
{
	unsigned char a1, a2;
	
	a1 = inb(_MASTE_PIC_DATA);                        // save masks
	a2 = inb(_SLAVE_PIC_DATA);
 
	outb(_MASTE_PIC_COMMAND, 0x11);  // starts the initialization sequence (in cascade mode)
	outb(_SLAVE_PIC_COMMAND, 0x11);
	
	outb(_MASTE_PIC_DATA, 	0x20);                 // ICW2: Master PIC vector offset
	outb(_SLAVE_PIC_DATA, 		0x28);                 // ICW2: Slave PIC vector offset
	outb(_MASTE_PIC_DATA, 	4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(_SLAVE_PIC_DATA, 		2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(_MASTE_PIC_DATA,		0x03);			//ICW4		auto send EOI
	outb(_SLAVE_PIC_DATA,		0x03);			//ICW4		auto send EOI
 
	outb(_MASTE_PIC_DATA,		a1);   		// restore saved masks.
	outb(_SLAVE_PIC_DATA,		a2);
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
	__IDT_CS__ = __KERNEL_CS;
	
	set_trap_gate(0, &divide_error);
	set_trap_gate(1, &debug);
	set_intr_gate(2, &nmi);
	set_system_gate(3, &int3);

	set_trap_gate(8,	&double_fault);
	
	set_trap_gate(13,&general_protection);
	set_trap_gate(14, &page_fault);
	
	set_intr_gate(0x20, &clockirq);
	set_intr_gate(0x21, &kbdirq);
	
	for (int i = 0x22; i < 0xff; i++)
	{
		set_intr_gate(i, &common_irq);
	}
	
	__asm {
		lidt idt_desc
	}
	
	init_8259_pic();
	//init_clock();
	//
	__asm {
		sti
	}
	return;
}
