#include "../common/msrnames.h"

#include <intrin.h>

static int x = 0;
static int y = 3;
static char str[0x20];


#define CPUID_FEAT_EDX_APIC		(1<<9)
#define CPUID_FEAT_ECX_X2APIC	(1<<21)

void GetCPUFamily(const char *out);
void APIC_test(void);
void Halt(void);
void __cdecl printf(const char* format, ...);

void __fastcall loader_main()
{
	//eax, ebx, ecx, edx
	GetCPUFamily(str);
	printf("Vendor: %s\n", str);

	APIC_test();

	__halt();
	for (int i = 0; i < 16; i++)
	{
		x += y;
		y += x;
	}
}
