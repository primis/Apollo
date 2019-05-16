;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2019 Apollo Project Developers            ;;
;; For terms, see LICENSE                        ;;
;; multiboot.s - talk to multiboot               ;;
;; This will eventually have multiboot 2 as well ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[GLOBAL mboot]
[EXTERN higherhalf]

; Multiboot Macro Definitions
MULTIBOOT_PAGE_ALIGN    equ 1<<0
MULTIBOOT_MEMORY_INFO   equ 1<<1
MULTIBOOT_VIDEO_MODE    equ 1<<2
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002


MULTIBOOT_HEADER_FLAGS  equ (MULTIBOOT_PAGE_ALIGN + MULTIBOOT_MEMORY_INFO)
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Multiboot Header
;==================
section .init

align 32
; This is the multiboot header.
mboot:
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM

global _start:function _start.end-_start
_start:
    mov eax, pd                 ; Move magic number into pd
    mov dword [eax], pt + 3     ; addrs 0 - 0x400000
    mov dword [eax+0xC00], pt+3 ; addrs 0xC0000000 - 0xC0400000
    
    ;; Loop through all 1024 pages in pt, set them to identity mapped
    mov edx, pt                 ;
    mov ecx, 0                  ; start a loop
.loop:
    mov eax, ecx                ; grab loop number
    shl eax, 12                 ;
    or  eax, 3                  ; Bit twiddling
    mov [edx+ecx*4], eax        ; pt[ecx * sizeof(entry)] -> tmp

    inc ecx                     ;
    cmp ecx, 1024               ; End condition
    jnz .loop                   ; loop back otherwise

    mov eax, pd+3               ; load page directory
    mov cr3, eax                ; load into cr3
    mov eax, cr0                ; grab cr0
    or  eax, 0x80000000         ; set PG Bit
    mov cr0, eax                ; push cr0 back
    jmp higherhalf              ; start kernel proper
.end:                           ; end of function
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Multiboot Padding
section .init.bss nobits
pd: resb    0x1000  ; Descriptors
pt: resb    0x1000  ; Page tabled

