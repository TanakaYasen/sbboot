#ifndef _MISC_H
#define _MISC_H

#include <stdint.h>

void Halt(void);
uint64_t GetCIP(void);
void __cdecl printf(const char* format, ...);

typedef union _CPUINFO {
	struct
	{
		int cpuinfo[4];
	};
	struct
	{
		int	eax, ebx, ecx, edx;
	};
}CPUINFO;


#endif