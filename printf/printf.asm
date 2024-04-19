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
; TODO: Isn't this function a literal wrapper of write syscall and the only difference
;       is in which registers it uses, why would you want it?
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
;          rdi = text end
;          al  = character
; Destroys: rdi, r8
;-----------------------------------------------------------------------

; TODO: Data in section text?
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
printNumber10: ; TODO: Why would you pass a pointer to number?
    push rbp
    ; TODO: There is a canonical entry:
    ;
    ;     | push rbp
    ;     | mov rbp, rsp
    ;
    ;     It makes it so that rbp points to the location of previous
    ;     rbp, making a kind of linked list on the stack.
    ;
    ;     This property is used by debugger to provide stack frame
    ;     information and it's also useful for local variable referencing.
    ;
    ;     Why are you not following the convention and instead pointing
    ;     rbp after your allocated buffer?
    ;
    ;     NOTE: It doesn't look like your making an especially great
    ;           effort to save registers, so it's probably not for optimization?

    push rdx
    sub rsp, DIGITS_BUFFER_SIZE     ; allocate buffer for digits
    mov rbp, rsp
    mov rax, [rdx]                  ; number
    mov rcx, 10                     ; base

.loop:                          ; TODO: Is .loop the best name you came up with?
    xor rdx, rdx
    div rcx

    add rdx, '0'
    mov [rbp], rdx
    inc rbp

    cmp rax, 0
    jne .loop                   ; TODO: loop .loop? if you use rcx instead of rax

.printloop:
    dec rbp
    mov al, [rbp]               ; TODO: would be lodsb with rdi? (movsb if you inline putc)
    call putc                   ; TODO: can this be a macro, call for every symbol seems sad?
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

    shr r10, cl                 ; TODO: Is everything else copy pasted? Make a macro and/or make this more efficient as it definitely can be...

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

; TODO: This is not a separate clause/function and shouldn't look like one, at least
;       put it inside myprintf and separated with newlines or make into a function
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

jtable:                         ; TODO: naming, don't use weird short reductions
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
    call putc
.ignore                         ; TODO: colon or no colon?
    inc rbx
    jmp .loop
.skipprint:                     ; TODO: proper spacing? Also, naming, you're really inconsistent

    inc rbx
    cmp byte[rbx], 0            ; TODO: please add a space before byte "byte [rbx]"
    je .end                     ;       not to be confused with C's array syntax
    je printPercent

    xor rax, rax
    mov al, byte[rbx]           ; TODO: what is this, some explanations please (like "my table's last symbol is x, so...")
    sub al, 'b'                 ;       also, what happens if table size changes? At least make a loud comment to change it!
    cmp al, 'x'-'a'             ;       Or make table include all alphabet for example.
    ja .ignore

    jmp [rax * 8 + jtable]

; TODO: This order is the most random thing ever
.endPrintArg:
    add rdx, 8             ; next arg

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

    pop r10

    ret

