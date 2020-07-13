
#include "header.h"

void loader_main()
{
	printf("Enter loader_main @0x%x == %x\n", loader_main, printf );
	setup_idt_and_test();
	printf("test IDT OK\n");
	setup_gdt();
	printf("GDT Reset OK\n");
	setup_idt();
	while (1);
	return;
}

