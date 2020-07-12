
#include "header.h"

int test_idt(int);

void loader_main()
{
	printf("Enter loader_main @0x%x == %x\n", loader_main, printf );
	setup_idt();
	printf("IDT OK\n");
	test_idt(0);
	printf("test IDT OK\n");
	
	setup_gdt();
	while (1);
	return;
}

