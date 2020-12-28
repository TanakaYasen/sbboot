function do_format(qNum)
	return (qNum>>39)&0x1ff, (qNum>>30)&0x1ff, (qNum>>21)&0x1ff, (qNum>>12)&0x1ff
end

function fmt(qNum)
	local a, b, c, d = do_format(qNum)
	print(a, b, c, d)
	print(string.format("%X\t%X\t%X\t%X", a, b, c, d))
	print(string.format("%X\t%X\t%X\t%X", a*8, b*8, c*8, d*8))
end

--local PTE_BASE = 0xFFFF878000000000
--FFFFF6FB7DBED000        FFFFF6FB7DA00000        FFFFF6FB40000000        FFFFF68000000000
local IDX = 0x1ED
local PTE_BASE = IDX
local PDE_BASE
local PPE_BASE
local PXE_BASE

PTE_BASE = (PTE_BASE << 39)
if IDX > 0xff then
	PTE_BASE = PTE_BASE | ~((1<<48)-1)
end


function get_pte(x)
	return (((x&0xffffffffffff)>>12)<<3)+PTE_BASE
end

function get_pde(x)
	return (((x&0xffffffffffff)>>21)<<3)+PDE_BASE
end

function get_ppe(x)
	return (((x&0xffffffffffff)>>30)<<3)+PPE_BASE
end

function get_pxe(x)
	return (((x&0xffffffffffff)>>39)<<3)+PXE_BASE
end

PDE_BASE = get_pte(PTE_BASE)
PPE_BASE = get_pte(PDE_BASE)
PXE_BASE = get_pte(PPE_BASE)

print(string.format("%X\t%X\t%X\t%X", PXE_BASE, PPE_BASE, PDE_BASE, PTE_BASE))

function pte(x)
	print(string.format("%X\t%X\t%X\t%X", get_pxe(x), get_ppe(x), get_pde(x), get_pte(x)))
end


function ptev2(x)
	local pte1 = get_pte(x)
	local pte2 = get_pte(pte1)
	local pte3 = get_pte(pte2)
	local pte4 = get_pte(pte3)
	print(string.format("%X\t%X\t%X\t%X", pte4, pte3, pte2, pte1))
end

function map(pte)
	print(string.format("%X", ((1<<48)-1) & (pte << 9)))
end