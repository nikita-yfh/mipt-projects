global myprintf

section .text

BUFFER_SIZE equ 1024

;-----------------------------------------------------------------------
; Prints buffer to stdout
; Entry: rdi - buffer to flush, rsi - length
; Destroys: rax, rdx, rdi, rsi
;-----------------------------------------------------------------------
puts:
    cmp  rsi, 0
    je .end
    mov  rax, 1
    mov  rdx, rsi ; rdx = length
    mov  rsi, rdi ; rsi -> buffer
    mov  rdi, 1 ; std out
    syscall ; print buffer
.end:
    ret

print:
.byte:
    jmp myprintf.ignore
.char:
    jmp myprintf.ignore
.number:
    jmp myprintf.ignore
.octal:
    jmp myprintf.ignore
.string:
    jmp myprintf.ignore
.hex:
    jmp myprintf.ignore
.percent:
    jmp myprintf.ignore

.jtable:
    dq print.byte     ; b
    dq print.char     ; c
    dq print.number   ; d
    times 10 dq myprintf.ignore
    dq print.octal    ; o
    times 3  dq myprintf.ignore
    dq print.string   ; s
    times 4  dq myprintf.ignore
    dq print.hex      ; x

myprintf:
    push r9
    push r8
    push rcx
    push rdx
    push rsi
    xor rcx, rcx             ; counter of arguments
    mov rdx, rdi             ; format string

    mov rbp, rsp             ; save stack head
    sub rsp, BUFFER_SIZE     ; allocate buffer
    mov rdi, rsp
    cld
.loop:
    cmp byte [rdx], 0
    je .end
    cmp byte [rdx], '%'
    jne .skipformat

    mov rdi, rsp
    mov rsi, rdx
    call puts
    
    inc rdi
    cmp byte[rdx], 0
    je .end
    cmp byte[rdx], '%'
    je print.percent
    
    xor rax, rax
    mov al, byte[rdx]
    sub al, 'b'
    cmp al, 'x'-'a'
    jb .ignore

    mov rax, print.jtable[rax]
    jmp rax
    inc rcx            ; counter of args

    .skipformat:

    mov rdi, rsp
    mov rsi, rdx
    movsb

    .ignore:

    inc rdx
    jmp .loop
.end:
    mov rax, rcx        ; return

    pop rsi
    pop rdx
    pop rcx
    pop r8
    pop r9

    ret

