[org 0x7c00]

	jmp main
%include "print.asm"

section.acc.start

main:
	mov ax, cs
	mov ds, ax
	mov es, ax
	
	; ��ӡ������Ϣ
	mov bx, BOOT_MESSAGE
	call print_string

	; ��ʼ��ȡ
	call disk_load
	call disk_load2

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

disk_load2:
    mov bx, 0xA000  ; �Ѵ���ָ�������е����ݼ����ڴ��е� 0x0000(ES):0x9000(BX) ��
    mov ah, 0x02    ; BIOS ��ȡ�����ķ���
    mov al, 11h       ; ��ȡ 17������
    mov ch, 0x00    ; CHS �е� cylinder Ϊ 0
    mov cl, 0x03    ; �ӵ� 2 ��������ʼ���������� bootsect �����������
    mov dl, 0x00
    mov dh, 0x00    ; CHS �е� head Ϊ 0
    int 0x13        ; ʹ�� BIOS 13 ���жϿ�ʼ�Ӵ��̶����ݵ��ڴ�
	ret


;#define FLOPPY_144_SECTORS_PER_TRACK 18
;void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
;{
;    *cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
;    *head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
;    *sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
;}
    
disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string
    jmp $


; ���ڴ�ӡ���ַ�����0x0a �� 0x0d �ֱ��ǻ��к� Enter�������ȥ����
; ��һ������һ�´�ӡ������Ч�����������Է�������������ַ������
BOOT_MESSAGE:
    db 'System is booting...', 0x0a, 0x0d, 0
BOOT_SUCCESS:
    db 'The kernel has been loaded.', 0x0a, 0x0d, 0

DISK_ERROR_MSG db "Disk read error!", 0

times 510-($-$$) db 0
dw 0xaa55
