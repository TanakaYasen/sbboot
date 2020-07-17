static int x = 0;
static int y = 3;

void __fastcall loader_main()
{
	for (int i = 0; i < 16; i++)
	{
		x += y;
		y += x;
	}
}
