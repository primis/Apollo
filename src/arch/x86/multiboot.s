;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (c) 2019 Apollo Project Developers            ;;
;; For terms, see LICENSE                        ;;
;; multiboot.s - talk to multiboot               ;;
;; This will eventually have multiboot 2 as well ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[GLOBAL mboot]

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

; Multiboot Padding
section .init.bss nobits
pd: resb    0x1000
pt: resb    0x1000

