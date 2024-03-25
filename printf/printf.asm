global myprintf

section .text

BUFFER_SIZE equ 1024
DIGITS_BUFFER_SIZE equ 64

;-----------------------------------------------------------------------
; Prints buffer to stdout
; Entry:   r8  = buffer length
;          r9  = buffer begin
;          rbp = buffer end
;          
; Destroys: rdi, r8
;-----------------------------------------------------------------------
flush:
    push rax
    push rdx
    push rsi

    cmp r8, 0
    je .end                ; if length == 0
    mov rax, 1
    mov rdx, r8            ; rdx = length
    mov rsi, r9            ; rsi = buffer_begin
    mov rdi, 1             ; std out
    syscall                ; print buffer
    mov r8, 0
    mov rdi, r9            ; rdi = buffer_begin
.end:

    pop rsi
    pop rdx
    pop rax

    ret


;-----------------------------------------------------------------------
; Puts character to buffer. If buffer is full, buffer will be flushed.
; Entry:   r8  = buffer length
;          rbp = buffer end
;          rdi = text end
;          al  = character
; Destroys: rdi, r8
;-----------------------------------------------------------------------

digits: db "0123456789abcdef"

putc:
    stosb
    inc r8                 ; buffer_length++
    cmp r8, BUFFER_SIZE
    jle .skipflush
    call flush
.skipflush:
    ret
    
;-----------------------------------------------------------------------
; Prints number with decimal base
; Entry:    rdx = pointer to number
; Destroys: rax
;-----------------------------------------------------------------------
printNumber10:
    push rbp
    push rdx
    sub rsp, DIGITS_BUFFER_SIZE     ; allocate buffer for digits
    mov rbp, rsp
    mov rax, [rdx]                  ; number
    mov rcx, 10                     ; base

.loop:
    xor rdx, rdx
    div rcx

    add rdx, '0'
    mov [rbp], rdx
    inc rbp
    
    cmp rax, 0
    jne .loop

.printloop:
    dec rbp
    mov al, [rbp]
    call putc
    cmp rbp, rsp
    jne .printloop

    add rsp, DIGITS_BUFFER_SIZE     ; free memory
    pop rdx
    pop rbp
    ret

;-----------------------------------------------------------------------
; Prints number with 2^n base
; Entry:    cl  = base shift increment
;           rsi = base mask
;           rdx = pointer to number
; Destroys: rax
;-----------------------------------------------------------------------
printNumber2N:
    push rbp
    push r10
    sub rsp, DIGITS_BUFFER_SIZE     ; allocate buffer for digits
    mov rbp, rsp
    mov r10, [rdx]          ; number

.loop:
    mov rax, r10
    and rax, rsi
    mov rax, [digits+rax]
    mov [rbp], rax
    inc rbp
    
    shr r10, cl
    
    cmp r10, 0
    jne .loop

.printloop:
    dec rbp
    mov al, [rbp]
    call putc
    cmp rbp, rsp
    jne .printloop

    add rsp, DIGITS_BUFFER_SIZE     ; free memory
    pop r10
    pop rbp
    ret

printByte:
    mov al, '0'
    call putc
    mov al, 'b'
    call putc
    mov rsi, 1
    mov cl,  1
    call printNumber2N
    jmp myprintf.endPrintArg

printChar:
    mov al, [rdx]
    call putc
    jmp myprintf.endPrintArg

printNumber:
    call printNumber10
    jmp myprintf.endPrintArg

printOctal:
    mov al, '0'
    call putc
    mov al, 'o'
    call putc
    mov rsi, 7
    mov cl,  3
    call printNumber2N
    jmp myprintf.endPrintArg

printString:
    mov rcx, [rdx]
.strloop:
    cmp byte [rcx], 0
    je .strskip
    mov al, [rcx]
    call putc
    inc rcx
    jmp .strloop
.strskip:
    jmp myprintf.endPrintArg

printHex:
    mov al, '0'
    call putc
    mov al, 'x'
    call putc
    mov rsi, 0xf
    mov cl,  4
    call printNumber2N
    jmp myprintf.endPrintArg

printPercent:
    mov al, '%'
    call putc
    jmp myprintf.ignore

jtable:
    dq printByte     ; b
    dq printChar     ; c
    dq printNumber   ; d
    times 10 dq myprintf.endPrintArg
    dq printOctal    ; o
    times 3  dq myprintf.endPrintArg
    dq printString   ; s
    times 4  dq myprintf.endPrintArg
    dq printHex      ; x

myprintf:
    push r9
    push r8
    push rcx
    push rdx
    push rsi

    mov rdx, rsp             ; current argument
    mov rbx, rdi             ; format string

    mov rbp, rsp             ; save stack head
    sub rsp, BUFFER_SIZE     ; allocate buffer
    mov rdi, rsp
    mov r8, 0                ; buffer size
    mov r9, rsp              ; buffer begin
    cld

.loop:
    cmp byte [rbx], 0
    je .end

    cmp byte [rbx], '%'
    je .skipprint
    mov al, [rbx]
    call putc
    jmp .ignore
.skipprint:

    inc rbx
    cmp byte[rbx], 0
    je .end
    cmp byte[rbx], '%'
    je printPercent
    
    xor rax, rax
    mov al, byte[rbx]
    sub al, 'b'
    cmp al, 'x'-'a'
    ja .ignore

    jmp [rax * 8 + jtable]


.endPrintArg:
    add rdx, 8             ; next arg

.ignore:
    inc rbx
    jmp .loop
.end:
    call flush             ; if buffer is not empty
    add rsp, BUFFER_SIZE

    mov rax, rdx        ; return
    sub rax, rbp
    shr rax, 3          ; rax = (rdx - rbp) / 8

    pop rsi
    pop rdx
    pop rcx
    pop r8
    pop r9

    ret

