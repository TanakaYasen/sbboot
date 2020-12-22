[BITS 64]

[section .text]

global GetCPUFamily
global Halt

GetCPUFamily:
	push rdi
	mov rdi, rcx 
	mov eax, 0
	cpuid
	mov dword [rdi], ebx
	mov dword [rdi+4], edx
	mov dword [rdi+8], ecx
	mov dword [rdi+0xc], 0
	pop rdi
	ret

Halt:
	hlt
	ret