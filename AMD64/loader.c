#include "../common/msrnames.h"
#include "misc.h"

#include <intrin.h>
#include <stdint.h>

static int x = 0;
static int y = 3;
static char str[0x20];


#define CPUID_FEAT_EDX_APIC		(1<<9)
#define CPUID_FEAT_ECX_X2APIC	(1<<21)

void GetCPUFamily(const char *out);
void mm_setup(void);
void APIC_test(void);

void __fastcall loader_main()
{
	uint64_t cip = GetCIP();
	printf("loader_main:%p\n", cip);
	//eax, ebx, ecx, edx
	GetCPUFamily(str);
	printf("Vendor: %s\n", str);

	mm_setup();
	APIC_test();

	__halt();
	

	for (int i = 0; i < 16; i++)
	{
		x += y;
		y += x;
	}
}
