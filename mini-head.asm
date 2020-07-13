[org 0x0000]

LOADAT	EQU	0xA000

struc DOS_HEADER
	.e_magic:	resb	2               ;-----------------------------说明---------------------------------------------
	.e_cblp:		resw 	1        ;事实上，从这里到e_res2无用部分可以用一句DUP(0)来完成，在这里编译器不会检查
	.e_cp:		resw	1        ;你结构的定义是否正确，这些变量名也是没有意义的，这样写只是有利于我们认识
	.e_crlc:		resw      1        ;PE结构而已，除了关键字，你可以任意写，因为它相当于写二进制型汇编代码
	.e_cparhdr:	resw      1        ;事实上，和直接写二进制是一样一样的，只是这样就轻松多了。当然了，
	.e_minalloc:	resw	1        ;这种方式的定义给我们带来的问题就是重定位，所以需要重定位的地方要小心了,
	.e_maxalloc:	resw 	1        ;你也可以把PE头部提上来到DOS头部里面来变形PE头，或者整个最小PE什么的，
	.e_ss:		resw	1        ;随你怎么高兴怎么玩，但是要保证e_lfanew定位的正确，你想怎么来就怎么来，你自由了！
	.e_sp:		resw	1        ;但是，没有绝对的自由，俗话说得好，任悟空本领再高，他也跳不出如来佛(OS)的手掌心啊
	.e_csum:	resw	1        ;------------------------------------------------------------------------------
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

struc FILE_HEADER
	.Machine:				resw	1
	.NumberOfSection:		resw 	1
	.TimeDateStamp:		resd	1
	.PointerToSymbolTable: 	resd      1
	.NumberOfSymbols:		resd      1
	.SizeOfOptionalHeader:	resw	1
	.Characteristics:			resw 	1
endstruc

struc OPTIONAL_HEADER
	.Magic:					resw	1
	.MajorLinkerVersion:		resb 	1
	.MinorLinkerVersion:		resb	1
	.SizeOfCode: 			resd      1
	.SizeOfInitializedData:	resd      1
	.SizeOfUninitializedData:	resd	1
	.AddressOfEntryPoint:	resd 	1
	.BaseOfCode:			resd 	1
	.BaseOfData:			resd 	1
	
	.ImageBase:				resd 	1
	.SectionAlignment:		resd 	1
	.FileAlignment:			resd 	1
	.MajorOperatingSystemVersion:			resw 	1
	.MinorOperatingSystemVersion:			resw 	1
	.MajorImageVersion:			resw 	1
	.MinorImageVersion:			resw 	1
	
	.Reserved1:				resd 	1
	.SizeOfImage:			resd 	1
	.SizeOfHeaders:				resd 	1
	.CheckSum:				resd 	1
	.Subsystem:				resw 	1
	.DllCharacteristics:		resw 	1
	.SizeOfStackReserve:				resd 	1
	.SizeOfStackCommit:			resd 	1
	.SizeOfHeapReserve:				resd 	1
	.SizeOfHeapCommit:			resd 	1
	.LoaderFlags:					resd 	1
	.NumberOfRvaAndSizes:			resd 	1
endstruc

struc NT_HEADER
    	.Signature				resd	1
    	.FileHeader				resb	FILE_HEADER_size
	.OptionalHeader			resb	OPTIONAL_HEADER_size
endstruc

struc SECTION_HEADER
    .Name 		resb	8
    .Misc			resd	1
    .VirtualAddress	resd	1
    .SizeOfRawData	resd	1
    .PointerToRawData resd	1
    .PointerToRelocations	resd	1
    .PointerToLinenumbers	resd	1
    .NumberOfRelocations	resw	1
    .NumberOfLinenumbers	resw	1
    .Characteristics			resd	1
endstruc

BASE equ 0x9000
	jmp main

align 8			; I GUESS:	8-bytes alignment donates better performance
gdt_table_start:
	;Intel规定段描述表的第一个描述符必须为空0
	gdt_null:
		dd 0h
		dd 0h
		
	gdt_data_idx equ $-gdt_table_start
	gdt_data:
		dw 07ffh	;段界限
		dw 1h		;段基地址0-15位
		db 0h		;段基地址16-23位
		db 10010010b	;段描述符的第六个字节属性（数据段可读可写）
		db 11000000b	;段描述符的第七个字节属性
		db 0		;段描述符的最后一个字节，也就是段基地址的第二部分
		
	gdt_code_idx equ $-gdt_table_start
	gdt_code:
		dw 07ffh	;段界限 0:15
		dw 0h		;段基地址0:15位
		db 0h		;段基地址16:23位
		db 10011010b	;P, DPL, S, Type
		db 11000000b	;G, D/B, L, AVL, SegLimit19:16
		db 0		;段基地址24-32位
		
	gdt_data_flat_idx equ $-gdt_table_start
	gdt_data_flat:
		dw 0xffffffff	;段界限
		dw 0h		;段基地址0-18位
		db 0h		;段基地址19-23位
		db 10010010b	;段描述符的第六个字节属性（数据段可读可写）
		db 11001111b	;段描述符的第七个字节属性
		db 0		;段描述符的最后一个字节，也就是段基地址的第二部分
		
	gdt_video_idx equ $-gdt_table_start
	gdt_video:
		dw 0ffh		;显存段界限1M
		dw 8000h
		db 0bh
		db 10010010b
		db 11000000b
		db 0
gdt_table_end:

gdtr_addr:								;6 Bytes
	dw gdt_table_end-gdt_table_start-1	;段描述表长度
	dd gdt_table_start+BASE			;段描述表基地址
	

main:
	xor eax, eax				;初始化数据段描述符的基地址
	mov eax, BASE
	mov word [gdt_data+2],ax
	shr eax,16
	mov byte [gdt_data+4],al
	mov byte [gdt_data+7],ah
	
	cli
	lgdt [gdtr_addr]			;让cpu读取gdtr_addr所指向内存内容

;	enable_A20:				;A20地址线开启, 不开也没问题 92h本来就是开的
		in al,92h
		or al,00000010b
		out 92h,al

	; 在此之前测试 一下这个
	
;	enter_pmode:			;进入保护模式
		mov eax, cr0
		or eax, 1
		mov cr0, eax		;此时已经是保护模式了
		
		;mov ax, gdt_data_flat_idx; mov ds, ax ;mov eax, 0x100001; mov byte [eax], 22h; 	ds base,limit都设好了
		
		;跳转到保护模式中
		jmp gdt_code_idx:code_32+BASE

[BITS 32]
	;在保护模式中打印字符
	data_32:
		msg db 'hello world'
		len equ $-msg
	code_32:
		mov ecx, [msg]
		; 原来的实模式的segment 还会保留，即 base=ds<<4, limit =0xffff.
		; ds:0x0900, dh=0x00009300, dl=0x9000ffff, valid=7
       	;	 Data segment, base=0x00009000, limit=0x0000ffff, Read/Write, Accessed
		; mov ds, ax 之后，才会刷新 segment
		mov ax, gdt_data_idx
		mov ds, ax
		; ds:0x0008, dh=0x00c09300, dl=0x900007ff, valid=1
        	;	Data segment, base=0x00009000, limit=0x007fffff, Read/Write, Accessed
		mov edx, [msg]	;assert ecx==edx
		
;	err_gdt:
;		mov dword [gdt_data+2], 0x0000
;		mov esp, [msg]	;写描述符内存不会刷新 segment 所以	esp==edx
		
		; refresh ds
		;mov ax, gdt_data_idx
		;mov ds, ax		;如果err_gdt	的 mov dword [gdt_data+2], 0xff00 被执行，则此句会异常，说明gdt segment-descriptor被修改后直接mov ds, ax后有问题
		;lgdt [gdtr_addr]	;即使重载 lgdt 则没用
		;mov ds, ax
		;mov ebp, [msg]
	
	; print		hello
		xor	ecx, ecx
		mov ax,gdt_video_idx
		mov es,ax
		mov cx,len
		mov edi,0
		mov bx, data_32
		mov ah,0ch
	s:	mov al,[ds:bx]
		mov [es:edi],al
		mov [es:edi+1],ah
		inc bx
		add edi,2
		loop s
		
		; enter flat mode
		mov ax, gdt_data_flat_idx
		mov ds, ax
		
paging:
		mov esi, 0x00005000
		mov ecx, 0x400	
	clearPDEs:
		mov dword [esi], 0
		add esi, 4
		loop clearPDEs
		; assert eax==0x6000
		
	;	0x00009000 -> 0x9000
	;	0000 0000 00 		00 0000 1001	...
	;		0x0				9
	;	0xf0009000	-> 0x9000
	;	1111 0000 00 		00 0000 1001	...
	;		0x3c0			9
	; set PDE; only 2
		mov esi, 0x00005000
		mov edx, 06h
		shl	edx, 12
		or edx, 00011b	; v, G, PS, 0, A, PCD, PWT, U/S, R/W, P
		mov [esi], edx
		
		mov edx, 07h
		shl	edx, 12
		or edx, 00011b	; v, G, PS, 0, A, PCD, PWT, U/S, R/W, P
		mov [esi+0x3c0*4], edx
	
	; mapping
		mov ecx, 0x400
		mov ebx, 0		;pfn	Page Frame Number
	lp_map:
		mov edx, ebx
		shl edx, 12
		or edx,  010b		; not Present
		mov [0x00007000+ebx*4], edx
		or edx,  011b
		mov [0x00006000+ebx*4], edx
		inc ebx
		loop lp_map
		
	; paging
		mov eax, 0x00005000
		mov cr3, eax
		
		mov eax, cr0
		or eax, 0x80000000
		mov cr0, eax
		
		; here to x /100b 0x00009000 ; visible
		; here to x /100b 0xf0009000 ;not visible
		
		mov edx, (9*0x1000) | 011b	;//set present!
		mov [0x00007000+9*4], edx	; only 0xf0009xxx' s page present
		; here to x /100b 0xf0009000	;be visible
		
		; x /4098b 0xf0009000
		; bx_dbg_read_linear: physical address not available for linear 0x00000000f000a000
		
		; load PE@ 0x100000
		
		; get EntryPoint
		mov esi, LOADAT										;esi = base
		mov edi, [esi + DOS_HEADER.e_lfanew]					;
		add edi, LOADAT										;edi = ntheader
		mov edx, [edi+NT_HEADER.Signature]					;Sig; 0x4550

		; first section		
		lea eax, [edi+NT_HEADER.FileHeader + FILE_HEADER.NumberOfSection]
		mov cx, [eax]
		lea eax, [edi +NT_HEADER.FileHeader + FILE_HEADER.SizeOfOptionalHeader]
		mov dx, [eax]
		
		mov ebx, edi
		add ebx, 4+FILE_HEADER_size
		add ebx, edx

LocateSection:
		push ecx
		
		mov eax,	[ebx+SECTION_HEADER.PointerToRawData]
		add eax,	LOADAT
		mov edx,	[ebx+SECTION_HEADER.VirtualAddress]
		add edx,	0x100000
		mov ecx, 	[ebx+SECTION_HEADER.Misc]
		
		call	copy_data
		add ebx,	SECTION_HEADER_size
		
		pop ecx
		loop LocateSection
		
		
		lea eax, [edi+NT_HEADER.OptionalHeader + OPTIONAL_HEADER.AddressOfEntryPoint]
		mov eax, [eax]
		add eax, 0x100000
		
		;set up esp and jmp
 		mov dx, gdt_data_flat_idx
		mov ss, dx
		mov esp, 0x200000
		
		jmp eax
		
copy_data:
		push esi
		push edi
		
		mov	esi,	eax
		mov	edi,	edx
sss:
		mov al, [esi]
		mov [edi], al 
		inc esi
		inc edi
		loop sss
		
		pop edi
		pop esi
		ret