[org 0x0000]

LOADAT	EQU	0xA000

struc DOS_HEADER
	.e_magic:	resb	2               ;-----------------------------˵��---------------------------------------------
	.e_cblp:		resw 	1        ;��ʵ�ϣ������ﵽe_res2���ò��ֿ�����һ��DUP(0)����ɣ������������������
	.e_cp:		resw	1        ;��ṹ�Ķ����Ƿ���ȷ����Щ������Ҳ��û������ģ�����дֻ��������������ʶ
	.e_crlc:		resw      1        ;PE�ṹ���ѣ����˹ؼ��֣����������д����Ϊ���൱��д�������ͻ�����
	.e_cparhdr:	resw      1        ;��ʵ�ϣ���ֱ��д��������һ��һ���ģ�ֻ�����������ɶ��ˡ���Ȼ�ˣ�
	.e_minalloc:	resw	1        ;���ַ�ʽ�Ķ�������Ǵ�������������ض�λ��������Ҫ�ض�λ�ĵط�ҪС����,
	.e_maxalloc:	resw 	1        ;��Ҳ���԰�PEͷ����������DOSͷ������������PEͷ������������СPEʲô�ģ�
	.e_ss:		resw	1        ;������ô������ô�棬����Ҫ��֤e_lfanew��λ����ȷ��������ô������ô�����������ˣ�
	.e_sp:		resw	1        ;���ǣ�û�о��Ե����ɣ��׻�˵�úã�����ձ����ٸߣ���Ҳ������������(OS)�������İ�
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
	;Intel�涨��������ĵ�һ������������Ϊ��0
	gdt_null:
		dd 0h
		dd 0h
		
	gdt_data_idx equ $-gdt_table_start
	gdt_data:
		dw 07ffh	;�ν���
		dw 1h		;�λ���ַ0-15λ
		db 0h		;�λ���ַ16-23λ
		db 10010010b	;���������ĵ������ֽ����ԣ����ݶοɶ���д��
		db 11000000b	;���������ĵ��߸��ֽ�����
		db 0		;�������������һ���ֽڣ�Ҳ���Ƕλ���ַ�ĵڶ�����
		
	gdt_code_idx equ $-gdt_table_start
	gdt_code:
		dw 07ffh	;�ν��� 0:15
		dw 0h		;�λ���ַ0:15λ
		db 0h		;�λ���ַ16:23λ
		db 10011010b	;P, DPL, S, Type
		db 11000000b	;G, D/B, L, AVL, SegLimit19:16
		db 0		;�λ���ַ24-32λ
		
	gdt_data_flat_idx equ $-gdt_table_start
	gdt_data_flat:
		dw 0xffffffff	;�ν���
		dw 0h		;�λ���ַ0-18λ
		db 0h		;�λ���ַ19-23λ
		db 10010010b	;���������ĵ������ֽ����ԣ����ݶοɶ���д��
		db 11001111b	;���������ĵ��߸��ֽ�����
		db 0		;�������������һ���ֽڣ�Ҳ���Ƕλ���ַ�ĵڶ�����
		
	gdt_video_idx equ $-gdt_table_start
	gdt_video:
		dw 0ffh		;�Դ�ν���1M
		dw 8000h
		db 0bh
		db 10010010b
		db 11000000b
		db 0
gdt_table_end:

gdtr_addr:								;6 Bytes
	dw gdt_table_end-gdt_table_start-1	;����������
	dd gdt_table_start+BASE			;�����������ַ
	

main:
	xor eax, eax				;��ʼ�����ݶ��������Ļ���ַ
	mov eax, BASE
	mov word [gdt_data+2],ax
	shr eax,16
	mov byte [gdt_data+4],al
	mov byte [gdt_data+7],ah
	
	cli
	lgdt [gdtr_addr]			;��cpu��ȡgdtr_addr��ָ���ڴ�����

;	enable_A20:				;A20��ַ�߿���, ����Ҳû���� 92h�������ǿ���
		in al,92h
		or al,00000010b
		out 92h,al

	; �ڴ�֮ǰ���� һ�����
	
;	enter_pmode:			;���뱣��ģʽ
		mov eax, cr0
		or eax, 1
		mov cr0, eax		;��ʱ�Ѿ��Ǳ���ģʽ��
		
		;mov ax, gdt_data_flat_idx; mov ds, ax ;mov eax, 0x100001; mov byte [eax], 22h; 	ds base,limit�������
		
		;��ת������ģʽ��
		jmp gdt_code_idx:code_32+BASE

[BITS 32]
	;�ڱ���ģʽ�д�ӡ�ַ�
	data_32:
		msg db 'hello world'
		len equ $-msg
	code_32:
		mov ecx, [msg]
		; ԭ����ʵģʽ��segment ���ᱣ������ base=ds<<4, limit =0xffff.
		; ds:0x0900, dh=0x00009300, dl=0x9000ffff, valid=7
       	;	 Data segment, base=0x00009000, limit=0x0000ffff, Read/Write, Accessed
		; mov ds, ax ֮�󣬲Ż�ˢ�� segment
		mov ax, gdt_data_idx
		mov ds, ax
		; ds:0x0008, dh=0x00c09300, dl=0x900007ff, valid=1
        	;	Data segment, base=0x00009000, limit=0x007fffff, Read/Write, Accessed
		mov edx, [msg]	;assert ecx==edx
		
;	err_gdt:
;		mov dword [gdt_data+2], 0x0000
;		mov esp, [msg]	;д�������ڴ治��ˢ�� segment ����	esp==edx
		
		; refresh ds
		;mov ax, gdt_data_idx
		;mov ds, ax		;���err_gdt	�� mov dword [gdt_data+2], 0xff00 ��ִ�У���˾���쳣��˵��gdt segment-descriptor���޸ĺ�ֱ��mov ds, ax��������
		;lgdt [gdtr_addr]	;��ʹ���� lgdt ��û��
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