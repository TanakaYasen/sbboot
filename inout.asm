[BITS 32]
section .text

global _outb@8
_outb@8:
	push ebp
	mov ebp, esp
	mov dx, [ebp+8]
	mov al, [ebp+12]
	out dx,al
	leave
	ret
	
global _outw@8
_outw@8:
	push ebp
	mov ebp, esp
	mov dx, [ebp+8]
	mov ax, [ebp+12]
	out dx,ax
	leave
	ret
	
global _inb@4
_inb@4:
	push ebp
	mov ebp, esp
	mov dx, [ebp+8]
	in al, dx
	leave
	ret
	
global _inw@4
_inw@4:
	push ebp
	mov ebp, esp
	mov dx, [ebp+8]
	in ax, dx
	leave
	ret