[org 0x7c00]

	jmp main
%include "print.asm"

main:
	mov ax, cs
	mov ds, ax
	mov es, ax
	
	; ��ӡ������Ϣ
	mov bx, BOOT_MESSAGE
	call print_string

	; ��ʼ��ȡ
	call disk_load

	; ��ӡ�ɹ���Ϣ
	mov bx, BOOT_SUCCESS
	call print_string
	
	mov ax, 0x900
	mov ds, ax
	jmp 0x900:0x0

disk_load:
	mov bx, 0x9000  ; �Ѵ���ָ�������е����ݼ����ڴ��е� 0x0000(ES):0x9000(BX) ��
	mov ah, 0x02    ; BIOS ��ȡ�����ķ���
	mov al, 2       ; ��ȡ 2 ������
	mov ch, 0x00    ; CHS �е� cylinder Ϊ 0
	mov dh, 0x00    ; CHS �е� head Ϊ 0
	mov cl, 0x02    ; �ӵ� 2 ��������ʼ���������� bootsect �����������
	int 0x13        ; ʹ�� BIOS 13 ���жϿ�ʼ�Ӵ��̶����ݵ��ڴ�
	jc disk_error   ; �жϵ���ʱ������ carry flag�����δ���ã������˴���
	mov bl, 2
	cmp bl, al      ; BIOS �ڶ�ȡʱ���������ȡ�������������� al
	jne disk_error  ; ��� al ��Ϊ 2����˵����ȡ�����˴���
	ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string
    jmp $


; ���ڴ�ӡ���ַ�����0x0a �� 0x0d �ֱ��ǻ��к� Enter�������ȥ����
; ��һ������һ�´�ӡ������Ч�����������Է�������������ַ������
BOOT_MESSAGE:
    db 'MBR...', 0x0a, 0x0d, 0
BOOT_SUCCESS:
    db 'next sector load done', 0x0a, 0x0d, 0

DISK_ERROR_MSG db "Disk read error!", 0

times 510-($-$$) db 0
dw 0xaa55
