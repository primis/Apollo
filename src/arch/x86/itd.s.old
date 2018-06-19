;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2016 Apollo Developers       ;;
;; idt.s - x86 CPU Interrupt Table  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[BITS 32]
[GLOBAL idtFlush]
[EXTERN isrHandler]
[GLOBAL isr0]
[GLOBAL isr1]
; Macros to make our lives easy

%macro ISR_NOERRCODE 1
    isr%1:
        cli
        push 0
        push %1
        jmp 0x08:isr_common_stub ; Force a long jump to keep length constant
%endmacro

%macro ISR_ERRCODE 1
    isr%1:
        cli
        nop
        nop
        push %1
        jmp 0x08:isr_common_stub
%endmacro

; IDT Flush -- Tell the CPU where interrupts go for servicing

idtFlush:               
    mov eax, [esp + 4]  ; Grab pointer from calling function
    lidt [eax]          ; Load into register
    sti                 ; New IDT installed, we can safely enable these
    ret                 ; Done.

; Interrupt Service Stub
isr_common_stub:

    ; Register Dump
    push ds         ; Push data segment at bottom of the stack
    push ebp        ; Set the order
    push ebx        ; EBX is the only register C doesn't clobber
    push edi
    push esi
    push edx
    push ecx
    push eax

    ; Data Segment Switch
    mov eax, 0x10   ; Kernel Data Segment
    mov  ds, eax
    mov  es, eax
    mov  fs, eax
    mov  gs, eax    ; Set all segment registers
    
    ; Stack Align  
    mov ebx, esp    ; Store stack before aligning it
    sub esp, 4      ; Add room for stack pointer
    and esp, 0xFFFFFFF0 ; Align 16
    mov [esp], ebx  ; Put the old stack pointer on the stack
    cld             ; SYSV ABI requires DF to be clear on function entry
    ; Call Handler
    call isrHandler ; C Code handler
    
    pop ebx         ; Recover Stack pointer
    mov esp, ebx    
    
    ; Data seg restore  
    mov eax, [esp + 28] ; Grab the data segment off the stack
    mov  ds, eax    ; Restore it back to the segments
    mov  es, eax
    mov  fs, eax
    mov  gs, eax    ; Restore the old segment
    
    ; Register_recover
    pop eax         ; We now pop everything off in reverse order.
    pop ecx
    pop edx
    pop esi
    pop edi         
    pop ebx         
    pop ebp         
    sti             ; Restore the interrupts
    add esp, 12     ; Remove DS, intNum and ErrCode
    iret

; Macro time!
%assign i 0

%rep 8
    ISR_NOERRCODE i
    %assign i i+1
%endrep

ISR_ERRCODE   8
ISR_NOERRCODE 9

%assign i 10
%rep 5
    ISR_ERRCODE i
    %assign i i+1
%endrep

%rep 240
    ISR_NOERRCODE i
    %assign i i+1
%endrep
