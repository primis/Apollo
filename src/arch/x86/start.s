;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2018 Apollo Project Developers ;;
;; start.s - x86 bootloader passover  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Multiboot Macro Definitions
MULTIBOOT_PAGE_ALIGN    equ 1<<0
MULTIBOOT_MEMORY_INFO   equ 1<<1
MULTIBOOT_VIDEO_MODE    equ 1<<2
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002


MULTIBOOT_HEADER_FLAGS  equ (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


; Kernel Stack size definition up here for visibility
KERNEL_STACK_SIZE       equ 0x4000



[BITS 32]
[GLOBAL mboot]
[GLOBAL start]
[EXTERN archInit]
[GLOBAL KERNEL_STACK]
[EXTERN code]
[EXTERN bss]
[EXTERN end]

; Multiboot Header
;==================
section .init

; This is the multiboot header.
mboot:
    align 32
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    dd mboot
    dd code
    dd bss
    dd end
    dd start



; Bootloader dropoff point
;==========================

start:
    ; Set up stack
    mov esp, KERNEL_STACK + KERNEL_STACK_SIZE
    push eax            ; Should be the multiboot magic number
    push ebx            ; Should be the multiboot header
    cli
    call archInit
    cli
    jmp $               ; endless loop
    hlt                 ; Deadlock Stop.
    jmp $               ; In case we get an NMI


; Multiboot Padding
section .init.bss nobits
pd: resb    0x1000
pt: resb    0x1000

; Stack
;=======
section .bss
ALIGN 8192
KERNEL_STACK:
    resb KERNEL_STACK_SIZE
