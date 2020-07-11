

const char msg[] = "Entering loader_main!                                                                                                                                                           ";

void display_message()
{
	unsigned short *p =(unsigned short *)0xb8000;
	for (int i = 0; i < sizeof(msg); i++)
	{
		*p++ = 0xd00 | msg[i];
	}
}

void setup_idt();

void loader_main()
{
	display_message();
	setup_idt();
	return;
}

