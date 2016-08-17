;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2016 Apollo Project Developers ;;
;; start.s - x86 bootloader passover  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Multiboot Macro Definitions
MULTIBOOT_PAGE_ALIGN    equ 1<<0
MULTIBOOT_MEMORY_INFO   equ 1<<1
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Kernel Stack size definition up here for visibility
KERNEL_STACK_SIZE       equ 0x4000

[BITS 32]
[GLOBAL mboot]
[GLOBAL start]
[EXTERN main]
[GLOBAL gdtFlush]


; Multiboot Header
;==================
section .text

mboot:
    ALIGN 4
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM

; Bootloader dropoff point
;==========================

start:
    mov esp, KERNEL_STACK + KERNEL_STACK_SIZE
    push ebx
    cli
    call main
    cli
    hlt

; GDT Loader
;============
gdtFlush:
    mov eax, [esp + 4]  ; Get the pointer from the stack
    lgdt [eax]          ; Load that into the MSR
    mov ax, 0x10        ; Kernel Code Segment
    mov ds, ax          ; Reload the segments
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush     ; Long jump to change to the new GDT
.flush:
    ret                 ; new GDT is now installed

; Stack
;=======
section .bss
ALIGN 32
KERNEL_STACK:
    resb KERNEL_STACK_SIZE
