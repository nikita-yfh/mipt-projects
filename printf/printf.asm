global myprintf

section .text

BUFFER_SIZE equ 1024
DIGITS_BUFFER_SIZE equ 64

;-----------------------------------------------------------------------
; Prints buffer to stdout
; Entry:   r8  = buffer length
;          r9  = buffer begin
;
; Destroys: rdi, r8
;-----------------------------------------------------------------------
flushBuffer:
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
; Puts character to buffer. If buffer is full, buffer will be flushBuffered.
; Entry:   r8  = buffer length
;          rdi = text end
;          al  = character
; Destroys: rdi, r8
;-----------------------------------------------------------------------
%macro printCharacter 0
    stosb
    inc r8                 ; buffer_length++
    cmp r8, BUFFER_SIZE
    jle %%skipflushBuffer
    call flushBuffer
%%skipflushBuffer:
%endmacro

;-----------------------------------------------------------------------
; Prints number with decimal base
; Entry:    r12 = number
; Destroys: rax, rcx
;-----------------------------------------------------------------------
printNumber10:
    push r11
    push rdx
    sub rsp, DIGITS_BUFFER_SIZE     ; allocate buffer for digits
    mov r11, rsp

    mov rax, r12                    ; number
    mov rcx, 10                     ; base

.divloop:
    xor rdx, rdx
    div rcx

    add rdx, '0'
    mov [r11], rdx
    inc r11

    cmp rax, 0
    jne .divloop                   ; TODO: loop .loop? if you use rcx instead of rax

.printloop:
    dec r11
    mov al, [r11]               ; TODO: would be lodsb with rdi? (movsb if you inline printCharacter)
    printCharacter                   ; TODO: can this be a macro, call for every symbol seems sad?
    cmp r11, rsp
    jne .printloop

    add rsp, DIGITS_BUFFER_SIZE     ; free memory
    pop rdx
    pop r11
    ret

;-----------------------------------------------------------------------
; Prints number with 2^n base
; Entry:    cl  = base shift increment
;           rsi = base mask
;           r12 = number
; Destroys: rax
;-----------------------------------------------------------------------
printNumber2N:
    push r11
    push r10
    sub rsp, DIGITS_BUFFER_SIZE     ; allocate buffer for digits
    mov r11, rsp
    mov r10, r12

.loop:
    mov rax, r10
    and rax, rsi
    mov rax, [digits+rax]
    mov [r11], rax
    inc r11

    shr r10, cl

    cmp r10, 0
    jne .loop

.printloop:
    dec r11
    mov al, [r11]
    printCharacter
    cmp r11, rsp
    jne .printloop

    add rsp, DIGITS_BUFFER_SIZE     ; free memory
    pop r10
    pop r11
    ret

myprintf:
    pop r10;                ; return address

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
    printCharacter
    jmp .noNextArg
.skipprint:

    inc rbx
    cmp byte [rbx], 0
    je .end
    cmp byte [rbx], '%'
    je .printPercent

    xor rax, rax
    mov al, byte [rbx]          ; TODO: what is this, some explanations please (like "my table's last symbol is x, so...")
    sub al, 'a'                 ;       also, what happens if table size changes? At least make a loud comment to change it!
    cmp al, 'z'-'a'             ;       Or make table include all alphabet for example.
    ja .noNextArg

    mov r12, [rdx]
    jmp [rax * 8 + .jumptable]

.nextArg:
    add rdx, 8             ; next arg
.noNextArg:
    inc rbx
    jmp .loop

.end:
    call flushBuffer             ; if buffer is not empty
    add rsp, BUFFER_SIZE

    mov rax, rdx        ; return
    sub rax, rbp
    shr rax, 3          ; rax = (rdx - rbp) / 8

    pop rsi
    pop rdx
    pop rcx
    pop r8
    pop r9

    push r10

    ret

;-----------------------------------------------------------------------
; print fuctions
; entry: r12 = argument
;-----------------------------------------------------------------------
.printByte:
    mov al, '0'
    printCharacter
    mov al, 'b'
    printCharacter
    mov rsi, 1
    mov cl,  1
    call printNumber2N
    jmp .nextArg

.printChar:
    mov al, r12b
    printCharacter
    jmp .nextArg

.printNumber:
    call printNumber10
    jmp .nextArg

.printOctal:
    mov al, '0'
    printCharacter
    mov al, 'o'
    printCharacter
    mov rsi, 7
    mov cl,  3
    call printNumber2N
    jmp .nextArg

.printString:
    mov rcx, r12
.strloop:
    cmp byte [rcx], 0
    je .strskip
    mov al, [rcx]
    printCharacter
    inc rcx
    jmp .strloop
.strskip:
    jmp .nextArg

.printHex:
    mov al, '0'
    printCharacter
    mov al, 'x'
    printCharacter
    mov rsi, 0xf
    mov cl,  4
    call printNumber2N
    jmp .nextArg

.printPercent:
    mov al, '%'
    printCharacter
    jmp .noNextArg

.jumptable:
    dq .nextArg       ; a
    dq .printByte     ; b
    dq .printChar     ; c
    dq .printNumber   ; d
    times 10 dq .nextArg
    dq .printOctal    ; o
    times 3  dq .nextArg
    dq .printString   ; s
    times 4  dq .nextArg
    dq .printHex      ; x
    times 2 dq .nextArg

section .data
    digits: db "0123456789abcdef"

