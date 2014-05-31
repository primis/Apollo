;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; (c) 2014 Apollo Develepors                                           ;
; init.s - multiboot starting point for x86 systems                    ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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
[GLOBAL gdt_flush]

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

gdt_flush:
	mov eax, [esp+4]	; Get Pointer off the stack
	lgdt [eax]			; Load the Pointer into the GDT register
	mov ax, 0x10	  	; 0x10 is the offset in the GDT to our data segment
	mov ds, ax			; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush		; 0x08 is the offset to our code segment: Far jump!
.flush:
	ret					; return to C calling function


;; BSS Datum
section .bss
ALIGN 32
KERNEL_STACK:
        resb KERNEL_STACK_SIZE
