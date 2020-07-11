; ��ӡ�ַ���
print_string:
    mov ah, 0x0e
    mov cl, [bx]
    cmp cl, 0
    jne bx_add
    ret

bx_add:
    mov al, [bx]
    int 0x10
    inc bx
    jmp print_string

; ��ӡ�������ֽڵ� 16 ���ƣ�����˳���Ƿ��ģ������û�ܽ����
print_hex:
    cmp dx, 0       ; ��Ҫ����ӡ���ֽڴ洢�� dx ��
    je  end         ; ��� dx Ϊ 0��˵���Ѿ���ӡ��ϣ���������

    mov cx, dx
    shr dx, 4       ; dx �߼����� 4 λ
    and cl, 0xf     ; cl = dx �ĵͰ�λ��������ͨ��������õ� cl �ĵ���λ��Ҳ���� dx �ĵ���λ
    cmp cl, 10
    jl less_ten     ; �����ʱ cl < 10��ִ�з��� less_then
    cmp cl, 10
    jge great_ten   ; �����ʱ cl >= 10��ִ�з��� great_then
end:
    ret

less_ten:
    add cl, 48      ; ���ֵ� ASCII ���Ǵ� 48 ��ʼ�ģ����� + 48 = ���ֶ�Ӧ�� ASCII ��
    mov al, cl
    mov ah, 0x0e
    int 0x10
    jmp print_hex   ; ������ӡ��һ���ַ�
great_ten:
    add cl, 55      ; ��д��ĸ�� ASCII ���Ǵ� 55 ��ʼ�ģ����� + 55 = ��ĸ��Ӧ�� ASCII ��
    mov al, cl
    mov ah, 0x0e
    int 0x10
    jmp print_hex