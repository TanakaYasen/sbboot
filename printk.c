#include<stdarg.h>						

static char *convert(unsigned int num, int base) 
{ 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 
	
	ptr = &buffer[49]; 
	*ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0); 
	
	return(ptr); 
}

#define VGABASE ((char *)0xb8000)
static char *video = VGABASE;
static int m_CursorX = 0, m_CursorY = 0;

static void scroll_up()
{
	if (m_CursorY  == 25)
	{
		for (int i = 0; i < 24; i++)
			for (int j = 0; j < 80; j++)
			{
				unsigned int pos = i * 80 + j;
				unsigned int pos_ = (i +1)*80 + j;
				
				VGABASE[pos * 2] =  VGABASE[pos_ * 2];
				VGABASE[pos * 2 + 1] = VGABASE[pos_ * 2 + 1];
			}
		m_CursorY--;
	}
}

static inline void putchar(char ch)
{
	scroll_up();	
	
	switch(ch)
	{
	case '\r':
	case '\n':
		m_CursorY ++;
		m_CursorX = 0;
		break;
	case '\t':
		m_CursorX = (m_CursorX / 8 + 1) * 8;
		if(m_CursorX >= 80)
		{
			m_CursorX -= 80;
			m_CursorY ++;
		}
		break;
	default:
		{
			unsigned int pos = m_CursorY * 80 + m_CursorX;
			VGABASE[pos * 2] =  ch;
			VGABASE[pos * 2 + 1] = 0x0d;
			m_CursorX++;
			if(m_CursorX >= 80)
			{
				m_CursorX -= 80;
				m_CursorY ++;
			}
		}
		break;
	}
}

static inline void puts(const char *str)
{
	while (*str)
	{
		putchar(*str);
		str++;
	}
}

void __cdecl printf(char* format, ...) 
{ 
	char *traverse; 
	int i; 
	char *s; 
	
	//Module 1: Initializing printf's arguments 
	va_list arg; 
	va_start(arg, format); 
	
	for(traverse = format; *traverse != '\0'; traverse++) 
	{ 
		while( *traverse != '%' && *traverse != '\0' ) 
		{ 
			putchar(*traverse);
			traverse++; 
		} 
		if (*traverse == '\0')
			break;
		
		traverse++; 
		
		//Module 2: Fetching and executing arguments
		switch(*traverse) 
		{ 
			case '%':
						putchar('%');
						break;
			case 'c' : i = va_arg(arg,int);		//Fetch char argument
						putchar(i);
						break; 
						
			case 'd' : i = va_arg(arg,int); 		//Fetch Decimal/Integer argument
						if(i<0) 
						{ 
							i = -i;
							putchar('-'); 
						} 
						puts(convert(i,10));
						break; 
						
			case 'o': i = va_arg(arg,unsigned int); //Fetch Octal representation
						puts(convert(i,8));
						break; 
						
			case 's': s = va_arg(arg,char *); 		//Fetch string
						puts(s); 
						break; 
						
			case 'x': i = va_arg(arg,unsigned int); //Fetch Hexadecimal representation
						puts(convert(i,16));
						break; 
		}
	} 
	
	//Module 3: Closing argument list to necessary clean-up
	va_end(arg); 
} 

