#include "../common/msrnames.h"

#include <intrin.h>

static int x = 0;
static int y = 3;
static char str[0x20];


#define CPUID_FEAT_EDX_APIC		(1<<9)

void GetCPUFamily(const char *out);
void Halt(void);
void __cdecl printf(const char* format, ...);

void __fastcall loader_main()
{
	//eax, ebx, ecx, edx
	int cpuInfo[4];
	GetCPUFamily(str);

	printf("Vendor: %s\n", str);

	__cpuid(cpuInfo, 1);

	int CPUBaseInfo = cpuInfo[0];
	int CPUXInfo = cpuInfo[3];
	printf("Family:%d Model:%d Stepping ID:%d\n",
        (CPUBaseInfo & 0x0F00)>>8,(CPUBaseInfo & 0xF0)>>4,CPUBaseInfo & 0xF);

	if (CPUXInfo & CPUID_FEAT_EDX_APIC)
	{
		printf("APIC\n");
	}

	__int64 msrv = __readmsr(IA32_APIC_BASE_MSR);
	printf("%x\n", msrv);

	msrv &= ~(0x1<<11);
	__writemsr(IA32_APIC_BASE_MSR, msrv);

	msrv |= 0x1<<11;
	__writemsr(IA32_APIC_BASE_MSR, msrv);



	__halt();
/*
    __cpuid(0x80000004, cpuInfo);
    printf("%#010x %#010x %#010x %#010x", 
    	cpuInfo[0],
    	cpuInfo[1],
    	cpuInfo[2],
    	cpuInfo[3]);
*/


	for (int i = 0; i < 16; i++)
	{
		x += y;
		y += x;
	}
}
