/*
 * crtentry.S
 *
 *  Created on: Aug 28, 2021
 *      Author: Korneliusz Osmenda
 */

.text
.section ".lo_init"
.global _start
 _start:
 	mov $0xff, %al
	outb %al, $0xa1
	outb %al, $0x21  // Disable interrupts as we change gdt
.extern gdt_pointer
	lgdt (gdt_pointer)
	jmp $0x08,$_pstart

_pstart:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	movl $(__stack_end-1), %esp
	movl $(__stack_end-1), %ebp
	push $0
.extern tss_init
	call tss_init
	jmp $0x08,$_cstart
