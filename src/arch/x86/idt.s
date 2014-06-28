;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; (c) 2014 Apollo Developers                                           ;
; idt.s - x86 Assembly for CPU interrupts                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;=========================[ Declarations ]=============================;
[BITS 32]
[GLOBAL idt_flush]
[EXTERN isrHandler]
[EXTERN irqHandler]

;===========================[ Macros ]=================================;
; All Of these macros should compile to exactly the same size.
; When tested in nasm, They are 13 bytes long each.

%macro ISR_NOERRCODE 1		; Most interrupts are software, they don't
	[GLOBAL isr%1]			; Have any error codes.
	isr%1:
		cli
		push byte 0					; Fake Error Code
		push byte %1				; Push Interrupt number
		jmp 0x08:isr_common_stub	; FORCE a Far jump to pad them all
%endmacro

%macro ISR_ERRCODE 1		; Some CPU Exceptions throw an error code
	[GLOBAL isr%1]			; We have to catch it.
	isr%1:
		cli
		nop				; Buffer to make all macros the same size.
		nop
		push byte %1
	    jmp 0x08:isr_common_stub
%endmacro

%macro IRQ 2				; IRQ's require additional C code.
	[GLOBAL irq%2]
	irq%2:
		cli
		push byte 0
		push byte %1
		jmp 0x08:irq_common_stub
%endmacro

;=============================[ Functions ]============================;

;----[ ISR Common Stub ]----;
isr_common_stub:            ; All The ISR macros call this function.
	pusha                   ; Save all the GP registers
	mov ax, ds              ; Get the data segment
	push eax                ; Save it to the stack
	mov ax, 0x10            ; Change to the kernel segment
	mov ds, ax              ; In All the segment registers
	mov es, ax              ;
	mov fs, ax              ;
	mov gs, ax              ;
	call isrHandler         ; Call the C Handler for interrupts
	pop eax                 ; Restore the original Segment
	mov ds, ax              ;
	mov es, ax              ;
	mov fs, ax              ;
	mov gs, ax              ;
	popa                    ; Restore the General Purpose Registers
	add esp, 8              ; Clean up the Stack
	sti                     ; Restore interrupts
	iret                    ; Return to the function we interrupted
;---------------------------;

;----[ IRQ Common Stub ]----;
irq_common_stub:            ; All The ISR macros call this function.
	pusha                   ; Save all the GP registers
	mov ax, ds              ; Get the data segment
	push eax                ; Save it to the stack
	mov ax, 0x10            ; Change to the kernel segment
	mov ds, ax              ; In All the segment registers
	mov es, ax              ;
	mov fs, ax              ;
	mov gs, ax              ;
	call irqHandler         ; Call the C Handler for the IRQ
	pop eax                 ; Restore the original Segment
	mov ds, ax              ;
	mov es, ax              ;
	mov fs, ax              ;
	mov gs, ax              ;
	popa                    ; Restore the General Purpose Registers
	add esp, 8              ; Clean up the Stack
	sti                     ; Restore interrupts
	iret                    ; Return to the function we interrupted
;---------------------------;

;------[ IDT Flush ]--------;
idt_flush:					; Set up a new Jump Table.
	mov eax, [esp + 4]		; Get the pointer location from the stack.
	lidt [eax]				; Load into the Control Register
    sti                     ; Restore Hardware Interrupts
	ret						;
;---------------------------;


;======================[ Interrupt Declarations ]======================;
%assign i 0				; Initialize the loop Variable

%rep 8					; The first 8 interrupts don't give error codes
	ISR_NOERRCODE i
	%assign i i+1
%endrep

ISR_ERRCODE   8			; #9 does
ISR_NOERRCODE 9			; #10 doesn't

%assign i 10			; Reset loop counter again
%rep 5					; The next five do give error codes
	ISR_ERRCODE i
	%assign i i+1
%endrep

%rep 17					; The Rest don't, but we have to stop for IRQ's
	ISR_NOERRCODE i
	%assign i i+1
%endrep

%rep 16					; 16 IRQ lines
	%assign j i-32		; j is the IRQ line number
	IRQ i, j			; Macros are fun :D
	%assign i i+1
%endrep

%rep 80				; First 48 were Special cases, the rest are not
	ISR_NOERRCODE i
	%assign i i+1
%endrep
