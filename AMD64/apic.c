
//https://blog.csdn.net/GerryLee93/article/details/106475013

#include "../common/msrnames.h"
#include "misc.h"

#include <intrin.h>
#include <stdint.h>

#define APIC_BASE	0xFFFFF00000000000

typedef uint32_t ULONG;
typedef void VOID;

static inline
ULONG
ApicRead(ULONG Offset)
{
    return *(volatile ULONG *)(APIC_BASE + Offset);
}

static inline
VOID ApicWrite(ULONG Offset, ULONG Value)
{
    *(volatile ULONG *)(APIC_BASE + Offset) = Value;
}


void APIC_test(void)
{
	CPUINFO	ci;
	__cpuid(ci.cpuinfo, 1);
	printf("Family:%d Model:%d Stepping ID:%d\n",
        (ci.eax & 0x0F00)>>8,(ci.eax & 0xF0)>>4, ci.eax & 0xF);

	printf(ci.edx &(1 << 9)?"APIC supported\n":"APIC not supported\n");
	printf(ci.ecx &(1<< 21)?"x2APIC supported\n":"x2APIC not supported\n");

	__int64 msrv = __readmsr(IA32_APIC_BASE_MSR);
	printf("%x\n", msrv);
#if 0
	printf("WriteMsr to disable it\n");

	//FEE00900: FEE00, physic page, 0x900=0b100100000000;
	// bit11's 1 = global APIC Enable;
	// bit10	x2APIC mode Enable
	// bit8's 1 = software APIC Enable; both=1 => APIC is truely enable
	printf("%x\n", msrv);
	msrv &= ~(0x1<<11);
	__writemsr(IA32_APIC_BASE_MSR, msrv);

	__cpuid(ci.cpuinfo, 1);
	printf(ci.edx &(1 << 9)?"APIC supported\n":"APIC not supported\n");		//now not supported..
	printf(ci.ecx &(1<< 21)?"x2APIC supported\n":"x2APIC not supported\n");

	printf("APIC Check over\n");

	// 在Pentium和P6处理器的APIC版本里，使用IA32_APIC_BASE寄存器的enable位来关闭APIC，要重新打开APIC必须经过reset重启。
	// 在xAPIC版本里（使用system bus通信的Pentium 4处理器上）重新开启APIC无须reset。
	msrv |= 0x1<<11;
	__writemsr(IA32_APIC_BASE_MSR, msrv);
#endif
	
	//mapva to pa
	uint32_t	*p = (uint32_t *)APIC_BASE;
	printf("LAPIC Version Register: %x\n", p[0x30>>2]);
	printf("Arbitration Priority Register: %x\n", p[0x90>>2]);
	printf("Processor Priority Register: %x\n", p[0xA0>>2]);
	printf("In-Service Register: %x\n", p[0x100>>2]);
	printf("In-Service Register: %x\n", p[0x110>>2]);

	printf("$Dbg:CIP=%p\n", GetCIP());
	ApicWrite(0x380, 123);
}
