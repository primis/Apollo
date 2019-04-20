;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2018 Apollo Project Developers ;;
;; For terms, see LICENSE             ;;
;; start.s - x86 bootloader passover  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Kernel Stack size definition up here for visibility
KERNEL_STACK_SIZE       equ 0x4000

[BITS 32]
[GLOBAL start]
[EXTERN arch_init]
[GLOBAL KERNEL_STACK]
[EXTERN code]
[EXTERN bss]
[EXTERN end]

;==========================;
; Bootloader dropoff point ;
;==========================;

start:
    ; Set up stack
    mov esp, KERNEL_STACK + KERNEL_STACK_SIZE
    push eax            ; Should be the multiboot magic number
    push ebx            ; Should be the multiboot header
    cli                 ; Make sure we don't get any errornous interrupts
    call arch_init      ; Pass control to C
    cli                 ;
    jmp $               ; endless loop
    hlt                 ; Deadlock Stop.
    jmp $               ; In case we get an NMI
;;;;;;;;;;;;;;;;;;;;;;;;;

; Stack
;=======
section .bss
ALIGN 8192
KERNEL_STACK:
    resb KERNEL_STACK_SIZE
