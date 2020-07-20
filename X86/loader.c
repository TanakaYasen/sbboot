
#include "header.h"

typedef struct _MEM_INFO
{
	uint16_t		ma0;
	uint16_t		ma;
	uint16_t		mb;
	uint16_t		mc;
	uint16_t		md;
	uint16_t		nRange;
	struct		{
		uint32_t		baselow;
		uint32_t		basehigh;
		uint32_t		lengthlow;
		uint32_t		lengthhigh;
		uint32_t		type;
	} ranges[8];
	
}MEM_INFO;

void __fastcall loader_main(const MEM_INFO *m_info)
{
	vesa_information();
	
	printf("%p\n", loader_main);
	printf("Enter loader_main @%p\n", loader_main);
	printf("m_info = %p\n", m_info);
	printf("%x contiguous KB above 1 MB.\n", m_info->ma0);

	printf("Extended:%xKB 1~16MB %x*64KB 16M~4G.\n", m_info->ma, m_info->mb);
	printf("Configured:%xKB 1~16MB %x*64KB 16M~4G.\n", m_info->mc, m_info->md);
	printf("\n");

#if 0
	for (int  i = 0; i < m_info->nRange; i++)
	{
		printf("addr=%x %x, length=%x %x\n"
			, m_info->ranges[i].basehigh, m_info->ranges[i].baselow
			, m_info->ranges[i].lengthhigh, m_info->ranges[i].lengthlow);
	}
#endif

	setup_idt_and_test();
	printf("test IDT OK\n");
	setup_gdt();
	printf("GDT Reset OK\n");
	setup_idt();
	printf("VGA test\n");
	while (1)
	{
		__asm { hlt };
	}
	
	demo_graphics();
	return;
}

