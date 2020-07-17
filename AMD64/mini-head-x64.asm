;	detect memory
;	
[org 0x9000]

BASE		EQU	0x9000
LOADAT	EQU	0xA000
PEBASE		EQU	0x100000

MSR_EFER	EQU	0xC0000080

struc ADDR_RANGE_DESC
	.BaseAddrLow	resd	1
	.BaseAddrHigh	resd	1
	.LengthLow		resd	1
	.LengthHigh		resd	1
	.Type			resd	1
endstruc

struc DOS_HEADER
	.e_magic:	resb	2
	.e_cblp:		resw 	1
	.e_cp:		resw	1
	.e_crlc:		resw      1
	.e_cparhdr:	resw      1
	.e_minalloc:	resw	1
	.e_maxalloc:	resw 	1
	.e_ss:		resw	1
	.e_sp:		resw	1
	.e_csum:	resw	1
	.e_ip:		resw	1
	.e_cs:		resw	1
	.e_lfarlc:		resw	1
	.e_ovno:	resw	1
	.e_res:		resw	4
	.e_oemid:	resw	1
	.e_oeminfo:	resw	1
	.e_res2:		resw	10
	.e_lfanew:	resd	1
endstruc

struc IMAGE_FILE_HEADER
	.Machine:				resw	1
	.NumberOfSection:		resw 	1
	.TimeDateStamp:		resd	1
	.PointerToSymbolTable: 	resd      1
	.NumberOfSymbols:		resd      1
	.SizeOfOptionalHeader:	resw	1
	.Characteristics:			resw 	1
endstruc

struc IMAGE_OPTIONAL_HEADER64
	.Magic:					resw	1
	.MajorLinkerVersion:		resb 	1
	.MinorLinkerVersion:		resb	1
	.SizeOfCode: 			resd      1
	.SizeOfInitializedData:	resd      1
	.SizeOfUninitializedData:	resd	1
	.AddressOfEntryPoint:	resd 	1
	.BaseOfCode:			resd 	1
	
	.ImageBase:				resq 	1
	.SectionAlignment:		resd 	1
	.FileAlignment:			resd 	1
	.MajorOperatingSystemVersion:			resw 	1
	.MinorOperatingSystemVersion:			resw 	1
	.MajorImageVersion:			resw 	1
	.MinorImageVersion:			resw 	1
	.MajorSubsystemVersion:		resw	1;
	.MinorSubsystemVersion:		resw	1;
	
	.Win32VersionValue:				resd 	1
	.SizeOfImage:					resd 	1
	.SizeOfHeaders:					resd 	1
	.CheckSum:						resd 	1
	.Subsystem:						resw 	1
	.DllCharacteristics:				resw 	1
	.SizeOfStackReserve:				resq 	1
	.SizeOfStackCommit:			resq 	1
	.SizeOfHeapReserve:				resq 	1
	.SizeOfHeapCommit:			resq 	1
	.LoaderFlags:					resd 	1
	.NumberOfRvaAndSizes:			resd 	1
	;DataDirectory
endstruc

struc IMAGE_NT_HEADERS64
    	.Signature				resd	1
    	.FileHeader				resb	IMAGE_FILE_HEADER_size
	.OptionalHeader			resb	IMAGE_OPTIONAL_HEADER64_size
endstruc

struc IMAGE_SECTION_HEADER
	.Name 					resb	8
	.Misc						resd	1
	.VirtualAddress			resd	1
	.SizeOfRawData			resd	1
	.PointerToRawData 		resd	1
	.PointerToRelocations		resd	1
	.PointerToLinenumbers	resd	1
	.NumberOfRelocations	resw	1
	.NumberOfLinenumbers	resw	1
	.Characteristics			resd	1
endstruc

BEGIN:
	jmp main

align 8			; I GUESS:	8-bytes alignment donates better performance
gdt_table_start:
	;Intel规定段描述表的第一个描述符必须为空0
	gdt_null:
		dd 0h
		dd 0h
		
	gdt_code_idx equ $-gdt_table_start
	gdt_code:
		dw 0ffffh	;段界限 0:15
		dw 0h		;段基地址0:15位
		db 0h		;段基地址16:23位
		db 10011010b	;P, DPL, S, Type
		db 01001111b	;G, D/B, L, AVL, SegLimit19:16
		db 0		;段基地址24-32位
		
	gdt_data_idx equ $-gdt_table_start
	gdt_data:
		dw 0ffffh	;段界限
		dw 0h		;段基地址0:15位
		db 0h		;段基地址16:23位
		db 10010010b	;段描述符的第六个字节属性（数据段可读可写）
		db 11001111b	;段描述符的第七个字节属性
		db 0		;段描述符的最后一个字节
		
gdt_table_end:

gdtr_addr:								;6 Bytes
	dw gdt_table_end-gdt_table_start-1	;段描述表长度
	dd gdt_table_start					;段描述表基地址
	

main:
	;ds == 0x900 mbr;
	mov	ax, 0
	mov 	ds, ax
	call disk_load_PE
	
	lgdt	[gdtr_addr]			;让cpu读取gdtr_addr所指向内存内容
	cli
	
	mov eax, 0x80000000    ; Set the A-register to 0x80000000.
	cpuid                  ; CPU identification.
	cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
	jb .NoLongMode

;	enable_A20:					;A20地址线开启, 不开也没问题 92h本来就是开的
		in al,92h
		or al,00000010b
		out 92h,al

	; 在此之前测试 一下这个
	
;	enter_pmode:
		mov	eax, cr0
		or		eax, 1		; CR0.PE = 1
		mov	cr0, eax		;此时已经是保护模式了
		
		;mov ax, gdt_data_flat_idx; mov ds, ax ;mov eax, 0x100001; mov byte [eax], 22h; 	ds base,limit都设好了
		jmp		gdt_code_idx:code_32

.NoLongMode:
		hlt

; http://ftp.lip6.fr/pub/mach/mach4/multiboot/mem64mb.html
probe_memory:
		;here ds is 0x900h
		mov	ah,	88h
		int 		15h
		mov	[MemAbove1K], ax
		
		mov	ax, 0xE801
		int		15h
		mov	[MemLargeConfigurationsAX], ax
		mov	[MemLargeConfigurationsBX], bx
		mov	[MemLargeConfigurationsCX], cx
		mov	[MemLargeConfigurationsDX], dx
		
		xor		ebx, ebx
		mov	ax, BASE >> 4
		mov	es, ax
		mov	di,	MemOverview
detect_iter:
		mov	edx, 'SMAP'
		mov	ecx, ADDR_RANGE_DESC_size
		mov	eax, 0xE820
		int		15h
		jc		detect_ok
		add		di, ADDR_RANGE_DESC_size
		inc		word [nRanges]
		jmp		detect_iter
		
detect_ok:
		ret


disk_load_PE:
		mov bx, LOADAT  ; 把磁盘指定扇区中的数据加载内存中的 0x0000(ES):0x9000(BX) 处
		mov ah, 0x02    ; BIOS 读取扇区的方法
		mov al, 11h       ; 读取 17个扇区
		mov ch, 0x00    ; CHS 中的 cylinder 为 0
		mov cl, 0x04    ; 从第 4 个扇区开始读（即接在 bootsect 后面的扇区）
		mov dl, 0x00
		mov dh, 0x00    ; CHS 中的 head 为 0
		int 0x13        ; 使用 BIOS 13 号中断开始从磁盘读数据到内存

		jc disk_error   ; 中断调用时会设置 carry flag，如果未设置，则发生了错误
		mov bl, 11h
		cmp bl, al      ; BIOS 在读取时会把真正读取到的扇区数赋给 al
		jne disk_error  ; 如果 al 不为 2，则说明读取发生了错误
		ret	
disk_error:
		hlt
	
MemAbove1K					dw	0
MemLargeConfigurationsAX		dw	0
MemLargeConfigurationsBX		dw	0
MemLargeConfigurationsCX		dw	0
MemLargeConfigurationsDX		dw	0
nRanges						dw	0
MemOverview					times 256	db	0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[BITS 32]
align 8			; I GUESS:	8-bytes alignment donates better performance
gdt_table_start_64:
	;Intel规定段描述表的第一个描述符必须为空0
	gdt_null_64:
		dd 0h
		dd 0h
	gdt_code_64:
		dw 0ffffh	;段界限已废除
		dw 0h		;段基地址已废除
		db 0h		;段基地址已废除
		db 10011010b	;P, DPL, S, Type					
		db 10111111b	;G, D/B, L, AVL, SegLimit19:16			;11111111	=>check_cs(0x0008): Both CS.L and CS.D_B bits enabled !
		db 0		;段基地址已废除
		
	gdt_data_64:
		dw 0ffffh	;段界限已废除
		dw 0h		;段基地址已废除
		db 0h		;段基地址已废除
		db 10010010b	;
		db 11001111b	;
		db 0		;;段基地址已废除
		
gdt_table_end_64:

gdtr_addr_64:								;6 Bytes
	dw gdt_table_end_64-gdt_table_start_64-1	;段描述表长度
	dd gdt_table_start_64					;段描述表基地址
	
	code_32:
		lgdt 	[gdtr_addr_64]				; reload

;typedef struct _PML4E
;{
;   union
;    {
;        struct
;        {
;            ULONG64 Present : 1;              // Must be 1, region invalid if 0.
;            ULONG64 ReadWrite : 1;            // If 0, writes not allowed.
;            ULONG64 UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
;            ULONG64 PageWriteThrough : 1;     // Determines the memory type used to access PDPT.
;            ULONG64 PageCacheDisable : 1;     // Determines the memory type used to access PDPT.
;            ULONG64 Accessed : 1;             // If 0, this entry has not been used for translation.
;            ULONG64 Ignored1 : 1;
;            ULONG64 PageSize : 1;             // Must be 0 for PML4E.
;            ULONG64 Ignored2 : 4;
;            ULONG64 PageFrameNumber : 36;     // The page frame number of the PDPT of this PML4E.
;            ULONG64 Reserved : 4;
;            ULONG64 Ignored3 : 11;
;            ULONG64 ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
;        };
;        ULONG64 Value;
;    };
;} PML4E, *PPML4E;

	IA_32e_paging:
		mov esi, 0x00002000
		mov ecx, 0x400	
		xor eax, eax
	clearPML4Es:
		mov dword [esi], 0
		add esi, 4
		loop clearPML4Es

;typedef struct _PDPTE
;{
;    union
;    {
;        struct
;        {
;            ULONG64 Present : 1;              // Must be 1, region invalid if 0.
;            ULONG64 ReadWrite : 1;            // If 0, writes not allowed.
;            ULONG64 UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
;            ULONG64 PageWriteThrough : 1;     // Determines the memory type used to access PD.
;            ULONG64 PageCacheDisable : 1;     // Determines the memory type used to access PD.
;            ULONG64 Accessed : 1;             // If 0, this entry has not been used for translation.
;            ULONG64 Ignored1 : 1;
;            ULONG64 PageSize : 1;             // If 1, this entry maps a 1GB page.
;            ULONG64 Ignored2 : 4;
;            ULONG64 PageFrameNumber : 36;     // The page frame number of the PD of this PDPTE.
;            ULONG64 Reserved : 4;
;            ULONG64 Ignored3 : 11;
;            ULONG64 ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
;        };
;        ULONG64 Value;
;    };
;} PDPTE, *PPDPTE;

		mov esi, 0x2000	
		mov edx, 03h
		shl	edx, 12
		or edx, 00100011b	; v, G, PS, 0, A, PCD, PWT, U/S, R/W, P
		mov [esi], edx

;typedef struct _PDE
;{
;    union
;    {
;        struct
;        {
;            ULONG64 Present : 1;              // Must be 1, region invalid if 0.
;            ULONG64 ReadWrite : 1;            // If 0, writes not allowed.
;            ULONG64 UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
;            ULONG64 PageWriteThrough : 1;     // Determines the memory type used to access PT.
;            ULONG64 PageCacheDisable : 1;     // Determines the memory type used to access PT.
;            ULONG64 Accessed : 1;             // If 0, this entry has not been used for translation.
;            ULONG64 Ignored1 : 1;
;            ULONG64 PageSize : 1;             // If 1, this entry maps a 2MB page.
;            ULONG64 Ignored2 : 4;
;            ULONG64 PageFrameNumber : 36;     // The page frame number of the PT of this PDE.
;            ULONG64 Reserved : 4;
;            ULONG64 Ignored3 : 11;
;            ULONG64 ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
;        };
;        ULONG64 Value;
;    };
;} PDE, *PPDE;

;	set_3000:
		mov esi, 0x3000
		mov edx, 04h
		shl edx, 12
		or edx, 00100011b
		mov [esi], edx
		add esi, 4
		mov [esi], eax
		add esi, 4
		
;	set_4000:
		mov esi, 0x4000
		mov edx, 05h
		shl edx, 12
		or edx, 00100011b
		mov [esi], edx
		add esi, 4
		mov [esi], eax
		add esi, 4
		
		mov edx, 06h
		shl edx, 12
		or edx, 00100011b
		mov [esi], edx
		add esi, 4
		mov [esi], eax
		add esi, 4

;typedef struct _PTE
;{
;    union
;    {
;        struct
;        {
;            ULONG64 Present : 1;              // Must be 1, region invalid if 0.
;            ULONG64 ReadWrite : 1;            // If 0, writes not allowed.
;            ULONG64 UserSupervisor : 1;       // If 0, user-mode accesses not allowed.
;            ULONG64 PageWriteThrough : 1;     // Determines the memory type used to access the memory.
;            ULONG64 PageCacheDisable : 1;     // Determines the memory type used to access the memory.
;            ULONG64 Accessed : 1;             // If 0, this entry has not been used for translation.
;            ULONG64 Dirty : 1;                // If 0, the memory backing this page has not been written to.
;            ULONG64 PageAccessType : 1;       // Determines the memory type used to access the memory.
;            ULONG64 Global: 1;                // If 1 and the PGE bit of CR4 is set, translations are global.
;            ULONG64 Ignored2 : 3;
;            ULONG64 PageFrameNumber : 36;     // The page frame number of the backing physical page.
;            ULONG64 Reserved : 4;
;            ULONG64 Ignored3 : 7;
;            ULONG64 ProtectionKey: 4;         // If the PKE bit of CR4 is set, determines the protection key.
;            ULONG64 ExecuteDisable : 1;       // If 1, instruction fetches not allowed.
;        };
;        ULONG64 Value;
;    };
;} PTE, *PPTE;

;5000~6000~7000~8000
		mov esi, 0x5000
		mov ecx, 0x800
		xor ebx, ebx
		
	lp_map_2M:
		mov edx, ebx
		shl edx, 12
		or edx, 00100011b	; 
		mov [esi], edx
		add esi, 4
		mov [esi], eax
		add esi, 4
		inc ebx
		loop lp_map_2M

	; paging
		mov eax, 0x00002000
		mov cr3, eax
		
		mov 	eax, cr4
		or		eax, 100000b		;PAE
		mov	cr4,	eax
		
		mov	ecx, MSR_EFER
		rdmsr
		or		eax, 100000001b	;LME, syscall
		wrmsr
		
		mov	eax, cr0
		or		eax, 0x80000000		;PG
		mov	cr0, eax
		jmp gdt_code_idx:code_64
		
[BITS 64]
	;在保护模式中打印字符
	data_64:
		msg db 'hello world'
		len equ $-msg
	code_64:
		mov ax, gdt_data_idx
		mov ds, ax
		mov es, ax
		
	; print		hellox
		mov rdi, 0xb8000
		mov rcx, len
		mov edi, 0
		mov rbx, data_64
		mov ah, 0ch
		mov rcx, len
		
	s:	mov al, [rbx]
		mov [rdi], al
		mov [rdi+1], ah
		inc rbx
		add rdi,2
		loop s
		
		; enter flat mode
		mov ax, gdt_data_idx
		mov ds, ax

		; load 	PE@ 0x100000
		; 
		mov	esi, LOADAT										;esi = base
		mov 	dx, [esi]
		cmp	dx, 'MZ'
		jne		load_pe_error
		
		mov	edi, [esi + DOS_HEADER.e_lfanew]				;
		add		edi, LOADAT									;edi = ntheader
		mov	edx, [edi+IMAGE_NT_HEADERS64.Signature]			;Sig; 0x4550
		
		cmp	edx, 0x4550
		jne		load_pe_error
		
		;;set up es for memcpy of section locationing
		mov	ax, ds
		mov	es, ax
		; first section
		lea		eax, [edi+IMAGE_NT_HEADERS64.FileHeader + IMAGE_FILE_HEADER.NumberOfSection]
		movzx	ecx, word [eax]
		lea		eax, [edi +IMAGE_NT_HEADERS64.FileHeader + IMAGE_FILE_HEADER.SizeOfOptionalHeader]
		movzx	edx, word [eax]
		
		lea		ebx, [edi + 4+IMAGE_FILE_HEADER_size]
		add		ebx, edx

LocateSections:
		push 	rcx
		
		mov	edx,	[ebx+IMAGE_SECTION_HEADER.PointerToRawData]		;
		add		edx,	LOADAT										;foa as source
		mov	eax,	[rbx+IMAGE_SECTION_HEADER.VirtualAddress]			;rva
		add		eax,	PEBASE											;va as dest
		mov	ecx,  	[rbx+IMAGE_SECTION_HEADER.Misc]					;section size
		
		call		memcpy
		add 	ebx,	IMAGE_SECTION_HEADER_size
		
		pop	rcx
		loop	LocateSections
		
;Get EntryPoint
		lea 		eax, [rdi+IMAGE_NT_HEADERS64.OptionalHeader + IMAGE_OPTIONAL_HEADER64.AddressOfEntryPoint]
		mov	eax, [eax]
		add		eax, PEBASE
		
;setup esp and jmp
 		mov	dx, gdt_data_idx
		mov	ss, dx
		mov	rsp, 0x200000
		mov	rcx,	MemAbove1K
		jmp 	rax

load_pe_error:
		hlt
		
; ecx = count,	eax = dest, edx =source;
memcpy:
		push	rsi
		push	rdi
		mov	edi, eax
		mov    	esi, edx
		push  	rcx
		shr    	ecx, 0x2
		rep		movsd
		pop	rcx
		and		ecx, 0x3
		rep 	movsb
		pop    	rdi
		pop    	rsi
		ret
		
dump_section:
		
		ret
times 1024-($-$$) 			nop
