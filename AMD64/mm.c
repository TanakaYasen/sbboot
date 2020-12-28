/*

ULONG64 get_pte_base()
{
    PHYSICAL_ADDRESS physical_address;
    ULONG64 pte_base = 0;
    physical_address.QuadPart = __readcr3() & 0xfffffffffffff000;   // ��ȡCR3�Ĵ����������12λ
    PULONG64 pxe_ptr = MmGetVirtualForPhysical(physical_address);   // ��ȡ�����ڵ������ַ - ҳ����ӳ��
    ULONG64 index = 0;
    // �����Ƚ�
    while((pxe_ptr[index] & 0xfffffffff000) != physical_address.QuadPart) {
        index++;
        if(index >= 512) {
            return 0;
        }
    }
    // ����pte��ַ
    pte_base = ((index + 0x1fffe00) << 39);
 
    return pte_base;
}


##################06338b000##################

1: kd> !pte 001`40000000
                                           VA 0000000140000000
PXE at FFFFF6FB7DBED000    PPE at FFFFF6FB7DA00028    PDE at FFFFF6FB40005000    PTE at FFFFF68000A00000
contains 4F3000006BD06867  contains 4F4000006FE87867  contains 0470000079908867  contains 82A0000061330025
pfn 6bd06     ---DA--UWEV  pfn 6fe87     ---DA--UWEV  pfn 79908     ---DA--UWEV  pfn 61330     ----A--UR-V

2: kd> !vtop 0 FFFFF68000A00000
Amd64VtoP: Virt fffff68000a00000, pagedir 000000006338b000
Amd64VtoP: PML4E 000000006338bf68
Amd64VtoP: PDPE 000000006338b000
Amd64VtoP: PDE 000000006bd06028
Amd64VtoP: PTE 000000006fe87000
Amd64VtoP: Mapped phys 0000000079908000

manually=================================
3: kd> r cr3
cr3=000000006338b000
3: kd> !dq 6338b000+8*1ed
#6338bf68 00000000`6338b863 00000000`6068c863
3: kd> !dq 6338b000+8*0
#6338b000 4f300000`6bd06867 00000000`00000000
3: kd> !dq 6bd06000+8*5
#6bd06028 4f400000`6fe87867 00000000`00000000
3: kd> !dq 6fe87000+8*0
#6fe87000 04700000`79908867 00000000`00000000

3: kd> !dq 79908000
#79908000 82a00000`61330025 2cd00000`5a32b025

3: kd> dq FFFFF68000A00000
fffff680`00a00000  82a00000`61330025 2cd00000`5a32b025




##################06e8c9000##################

1: kd> !pte 0140000000
                                           VA 0000000140000000
PXE at FFFFF6FB7DBED000    PPE at FFFFF6FB7DA00028    PDE at FFFFF6FB40005000    PTE at FFFFF68000A00000
contains 747000006EE48867  contains 317000006EB49867  contains 047000005824A867  contains 82A0000057E24025
pfn 6ee48     ---DA--UWEV  pfn 6eb49     ---DA--UWEV  pfn 5824a     ---DA--UWEV  pfn 57e24     ----A--UR-V

1: kd> !vtop 0 0140000000
Amd64VtoP: Virt 0000000140000000, pagedir 000000006e8c9000
Amd64VtoP: PML4E 000000006e8c9000
Amd64VtoP: PDPE 000000006ee48028
Amd64VtoP: PDE 000000006eb49000
Amd64VtoP: PTE 000000005824a000
Amd64VtoP: Mapped phys 0000000057e24000
Virtual address 140000000 translates to physical address 57e24000.



1: kd> !vtop 0 FFFFF68000A00000
Amd64VtoP: Virt fffff68000a00000, pagedir 000000006e8c9000
Amd64VtoP: PML4E 000000006e8c9f68
Amd64VtoP: PDPE 000000006e8c9000
Amd64VtoP: PDE 000000006ee48028
Amd64VtoP: PTE 000000006eb49000
Amd64VtoP: Mapped phys 000000005824a000




> lv4(0x140000000)
FFFFF6FB7DBED000        FFFFF6FB7DA00028        FFFFF6FB40005000        FFFFF68000A00000
> lv4(0xFFFFF68000A00000)
FFFFF6FB7DBEDF68        FFFFF6FB7DBED000        FFFFF6FB7DA00028        FFFFF6FB40005000
> lv4(0xFFFFF6FB40005000)
FFFFF6FB7DBEDF68        FFFFF6FB7DBEDF68        FFFFF6FB7DBED000        FFFFF6FB7DA00028
> lv4(0xFFFFF6FB7DA00028)
FFFFF6FB7DBEDF68        FFFFF6FB7DBEDF68        FFFFF6FB7DBEDF68        FFFFF6FB7DBED000
> lv4(0xFFFFF6FB7DBED000)
FFFFF6FB7DBEDF68        FFFFF6FB7DBEDF68        FFFFF6FB7DBEDF68        FFFFF6FB7DBEDF68
>

3: kd> !vtop 0 0xFFFFF6FB7DBEDF68
Amd64VtoP: Virt fffff6fb7dbedf68, pagedir 000000006338b000
Amd64VtoP: PML4E 000000006338bf68
Amd64VtoP: PDPE 000000006338bf68
Amd64VtoP: PDE 000000006338bf68
Amd64VtoP: PTE 000000006338bf68
Amd64VtoP: Mapped phys 000000006338bf68




���ۣ�ҳ��������ַ��cr3�޹أ�ֻ��ϵͳ���ŵĶ���

*/


//mov cr3, 0x1000000


#include <stdint.h>


void __cdecl printf(const char* format, ...);
typedef uint64_t		addr_t;
#define BASE 		0xFFFFF00000000000
#define PHY_BASE	0x2000

static inline addr_t get_pte_of_va(addr_t va)
{
	return (((va&0xffffffffffff) >> 12)<<3) + 0xFFFFF68000000000;
}


static void set_pte(addr_t *page_va, addr_t pfn)
{
	addr_t val = pfn << 12;
	val |= 0x23;
	*page_va = val;
}

void mm_setup(void)
{
	addr_t *Cr3 = (addr_t*)PHY_BASE;
	addr_t	vax = 0xFFFFF00000000000;
	
	Cr3[0x1ED] = (0x2<<12) | 0x23;	//self-map
	
	addr_t 	pte = get_pte_of_va(vax);
	addr_t	pde = get_pte_of_va(pte);
	addr_t	ppe = get_pte_of_va(pde);
	addr_t	pxe = get_pte_of_va(ppe);
	
	printf("%p %p %p %p\n",  pxe, ppe, pde, pte);
	
	set_pte((addr_t *)pxe, 0x400);
	set_pte((addr_t *)ppe, 0x401);
	set_pte((addr_t *)pde, 0x402);
	set_pte((addr_t *)pte, 0xfee00);
	
	printf("map done: %p\n", pxe);
}

void *mm_alloc()
{
	return NULL;
}