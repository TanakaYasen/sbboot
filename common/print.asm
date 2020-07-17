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