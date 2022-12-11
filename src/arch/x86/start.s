;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2018 - 2022  Apollo Project Developers ;;
;; For terms, see LICENSE                     ;;
;; start.s - x86 bootloader passover          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Kernel Stack size definition up here for visibility
KERNEL_STACK_SIZE       equ 0x4000

[BITS 32]
[GLOBAL start]
[EXTERN arch_init]
[GLOBAL KERNEL_STACK]
[EXTERN code]
[EXTERN bss]
[EXTERN end]

;============================;
; Bootloader's dropoff point ;
; Dropped here from start    ;
;============================;
section .text
global higherhalf:function higherhalf.end-higherhalf
higherhalf:
    mov esp, stack      ; Multiboot doesnt give us a valid stack
    xor ebp, ebp        ; Clear the stack frame
    push ebx            ; Should be multiboot header
    cli                 ; Make sure we don't get any errornous interrupts
    call arch_init      ; Pass control to C
    cli                 ;
    jmp $               ; endless loop
    hlt                 ; Deadlock Stop.
    jmp $               ; In case we get an NMI
.end:                   ;
;;;;;;;;;;;;;;;;;;;;;;;;;

; Stack
;=======
section .bss
ALIGN 8192
global stack_base
stack_base:
    resb KERNEL_STACK_SIZE
stack:  ; label for esp only (bottom of the stack)
