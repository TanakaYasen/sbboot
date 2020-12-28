
//https://blog.csdn.net/GerryLee93/article/details/106475013

#include "../common/msrnames.h"
#include <intrin.h>
#include <stdint.h>

void __cdecl printf(const char* format, ...);


#define APIC_BASE_VA	0x80000


typedef union CPUINFO {
	struct
	{
		int cpuinfo[4];
	};
	struct
	{
		int	eax, ebx, ecx, edx;
	};
}CPUINFO;

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

	// ��Pentium��P6��������APIC�汾�ʹ��IA32_APIC_BASE�Ĵ�����enableλ���ر�APIC��Ҫ���´�APIC���뾭��reset������
	// ��xAPIC�汾�ʹ��system busͨ�ŵ�Pentium 4�������ϣ����¿���APIC����reset��
	msrv |= 0x1<<11;
	__writemsr(IA32_APIC_BASE_MSR, msrv);
#endif
	
	//mapva to pa
	uint32_t		*p = (uint32_t *)0xFFFFF00000000000;
	for (int i = 0; i < 0x10; i++)
	{
		printf("%x\n", p[i]);
	}
	
}