[org 0x7c00]

	jmp main
%include "print.asm"

main:
	mov ax, cs
	mov ds, ax
	mov es, ax
	
	; 打印启动信息
	mov bx, BOOT_MESSAGE
	call print_string

	; 开始读取
	call disk_load

	; 打印成功信息
	mov bx, BOOT_SUCCESS
	call print_string
	
	mov ax, 0x900
	mov ds, ax
	jmp 0x900:0x0

disk_load:
	mov bx, 0x9000  ; 把磁盘指定扇区中的数据加载内存中的 0x0000(ES):0x9000(BX) 处
	mov ah, 0x02    ; BIOS 读取扇区的方法
	mov al, 2       ; 读取 2 个扇区
	mov ch, 0x00    ; CHS 中的 cylinder 为 0
	mov dh, 0x00    ; CHS 中的 head 为 0
	mov cl, 0x02    ; 从第 2 个扇区开始读（即接在 bootsect 后面的扇区）
	int 0x13        ; 使用 BIOS 13 号中断开始从磁盘读数据到内存
	jc disk_error   ; 中断调用时会设置 carry flag，如果未设置，则发生了错误
	mov bl, 2
	cmp bl, al      ; BIOS 在读取时会把真正读取到的扇区数赋给 al
	jne disk_error  ; 如果 al 不为 2，则说明读取发生了错误
	ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string
    jmp $


; 用于打印的字符串，0x0a 和 0x0d 分别是换行和 Enter，你可以去掉其
; 中一个来看一下打印出来的效果，这样可以方便你对这两个字符的理解
BOOT_MESSAGE:
    db 'MBR...', 0x0a, 0x0d, 0
BOOT_SUCCESS:
    db 'next sector load done', 0x0a, 0x0d, 0

DISK_ERROR_MSG db "Disk read error!", 0

times 510-($-$$) db 0
dw 0xaa55
