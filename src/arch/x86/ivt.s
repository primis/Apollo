;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2018 Apollo Project Developers    ;;
;; ivt.s - x86 Interrupts - the asm part ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[BITS 32]
[EXTERN interrupt_handler]
[GLOBAL idt_flush]
[GLOBAL isr0]
[GLOBAL isr1]

%macro ISR_NOERRCODE 1
    isr%1:
        cli
        push 0
        push %1
        jmp 0x08:isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
    isr%1:
        cli
        nop
        nop
        push %1
        jmp 0x08:isr_common_stub
%endmacro

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

isr_common_stub:
    pusha
    mov eax, ds
    push eax            ; Store Data Segment

    mov eax, 0x10       ; Kernel data selector
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    
    push esp
    
    call interrupt_handler
    add esp, 4
    pop eax             ; get back the old data segment
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    popa
    add esp, 8
    iret

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14

    ;; Repeat 241 times (to 255)
%assign i 15
%rep 241
    ISR_NOERRCODE i
    %assign i i+1
%endrep

