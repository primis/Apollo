;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2022 Apollo Project Developers    ;;
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
        push %1
        jmp 0x08:isr_no_err_stub
%endmacro

%macro ISR_ERRCODE 1
    isr%1:
        cli
        push %1
        jmp 0x08:isr_common_stub
%endmacro

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

isr_no_err_stub:
    push 0xDEADBEEF
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

;===================; Type      | Name 
ISR_NOERRCODE 0     ; Fault     | Divide By Zero
ISR_NOERRCODE 1     ; Trap      | Debug
ISR_NOERRCODE 2     ; Interrupt | Non-Maskable Interrupt
ISR_NOERRCODE 3     ; Trap      | Breakpoint
ISR_NOERRCODE 4     ; Trap      | Overflow
ISR_NOERRCODE 5     ; Fault     | Bound Range Exceeded
ISR_NOERRCODE 6     ; Fault     | Invalid Opcode
ISR_NOERRCODE 7     ; Fault     | Device Not Available
ISR_ERRCODE   8     ; Abort     | Double Fault
ISR_NOERRCODE 9     ; Fault     | Coprocesser Segment Overrun
ISR_ERRCODE   10    ; Fault     | Invalid TSS
ISR_ERRCODE   11    ; Fault     | Segment Not Present
ISR_ERRCODE   12    ; Fault     | Stack-Segment Fault
ISR_ERRCODE   13    ; Fault     | General Protection Fault
ISR_ERRCODE   14    ; Fault     | Page Fault
ISR_NOERRCODE 15    ;           | Reserved
ISR_NOERRCODE 16    ; Fault     | x87 Floating-Point Exception
ISR_ERRCODE   17    ; Fault     | Alignment Check
ISR_NOERRCODE 18    ; Abort     | Machine Check
ISR_NOERRCODE 19    ; Fault     | SIMD Floating Point Exception
ISR_NOERRCODE 20    ; Fault     | Virtualization Exception
ISR_ERRCODE   21    ; Fault     | Control Protection Exception
ISR_NOERRCODE 22    ;           | Reserved
ISR_NOERRCODE 23    ;           | Reserved
ISR_NOERRCODE 24    ;           | Reserved
ISR_NOERRCODE 25    ;           | Reserved
ISR_NOERRCODE 26    ;           | Reserved
ISR_NOERRCODE 27    ;           | Reserved 
ISR_NOERRCODE 28    ; Fault     | Hypervisor Injection Exception
ISR_ERRCODE   29    ; Fault     | VMM Communication Exception
ISR_NOERRCODE 30    ; Fault     | Security Exception
ISR_NOERRCODE 31    ;           | Reserved

;; Repeat 223 times (to 255) for non-CPU generated Interrupt Vectors
%assign i 32
%rep 223
    ISR_NOERRCODE i
    %assign i i+1
%endrep

