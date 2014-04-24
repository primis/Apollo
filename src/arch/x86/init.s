;; Declarations
; Multiboot
MULTIBOOT_PAGE_ALIGN    equ 1<<0
MULTIBOOT_MEMORY_INFO   equ 1<<1
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Misc. Declaratoins here
KERNEL_STACK_SIZE       equ 0x4000
KERNEL_VERSION          equ 1
KERENL_VERSION          equ 1

[BITS 32]
[GLOBAL mboot]
[GLOBAL start]
[EXTERN main]

;; CODE

section .text

;;;;;
; Tell Multiboot we exist.
;;;;;

mboot:
	ALIGN 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

;;;;;
; Bootloader drops us off here
;;;;;

start:
	mov esp, KERNEL_STACK + KERNEL_STACK_SIZE
	push ebx
	cli		; We Don't want to be interrupted until after setup
	call main
	cli		; By the time we hit this, the system has interrupts on.
	hlt		; Halt the CPU with interrupts off, this deadlocks the CPU

;; BSS Datum
section .bss
ALIGN 32
KERNEL_STACK:
        resb KERNEL_STACK_SIZE
