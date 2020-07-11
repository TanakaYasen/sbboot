; 打印字符串
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

; 打印两个个字节的 16 进制（不过顺序是反的，这个我没能解决）
print_hex:
    cmp dx, 0       ; 需要被打印的字节存储在 dx 中
    je  end         ; 如果 dx 为 0，说明已经打印完毕，方法返回

    mov cx, dx
    shr dx, 4       ; dx 逻辑右移 4 位
    and cl, 0xf     ; cl = dx 的低八位，这里再通过与操作得到 cl 的低四位，也就是 dx 的低四位
    cmp cl, 10
    jl less_ten     ; 如果此时 cl < 10，执行方法 less_then
    cmp cl, 10
    jge great_ten   ; 如果此时 cl >= 10，执行方法 great_then
end:
    ret

less_ten:
    add cl, 48      ; 数字的 ASCII 码是从 48 开始的，数字 + 48 = 数字对应的 ASCII 码
    mov al, cl
    mov ah, 0x0e
    int 0x10
    jmp print_hex   ; 继续打印下一个字符
great_ten:
    add cl, 55      ; 大写字母的 ASCII 码是从 55 开始的，数字 + 55 = 字母对应的 ASCII 码
    mov al, cl
    mov ah, 0x0e
    int 0x10
    jmp print_hex