/*
 * mbr.S
 *
 *  Created on: Aug 24, 2021
 *      Author: Korneliusz Osmenda
 */

#define xstr(s) str(s)
#define str(s) #s
.text
.code16
//.org 0x6c00
 .globl _start
 _start:
 	jmp begin

brDrive:
 .byte      0                   // Physical drive no.
brSector:
 .byte      2                   // Physical drive no.
brCylinder:
 .byte      0                   // Physical drive no.
brHead:
 .byte      0                   // Physical drive no.
brendSeg:
 .long      code_end                   // Physical drive no.


 begin:

 	cli                         // We do not want to be interrupted
	xor %ax, %ax                  // 0 AX
	mov %ax, %ds                  // Set Data Segment to 0
	mov %ax, %es                 // Set Extra Segment to 0
	mov %ax, %ss                 // Set Stack Segment to 0
	mov $0x7C00, %sp              // Set Stack Pointer to 0x7c00
 	mov %sp, %bp

	jmp $0x0,$LowStart
LowStart:

	sti
	mov	%dl, brDrive	//Drive number

	movl brendSeg, %eax
	shr $4, %eax
	movl %eax, brendSeg

	mov $0x07E0, %bx
	mov %bx, %es

LoopRead:
	mov $0x0201, %ax
	mov brCylinder , %ch
	mov brSector, %cl
	mov brHead, %dh
	mov brDrive, %dl
	mov $0, %bx
	int $0x13
	jnc noerror
loop:
	hlt
	jmp loop
noerror:

	add $1,brSector
	cmpb $19, brSector
	jne AddEnd
	movb $1, brSector
	add $1,brHead
	cmpb $2, brHead
	jne AddEnd
	movb $0, brHead
	add $1, brCylinder

AddEnd:
	mov %es,%ax
	add $32, %ax
	cmp %ax, brendSeg
	mov %ax, %es
	jge LoopRead


	mov     $0x2401, %ax                //--- A20-Gate Activate ---
	int     $0x15

	cli

	lgdt gdt_ptr
	mov %cr0, %eax
	orb $1, %al      // set PE (Protection Enable) bit in CR0 (Control Register 0)
	mov %eax, %cr0

	sti
	jmp $0x08,$0x7E00

gdt_ptr:
.word gdt_end - gdt -1
.long gdt
gdt:
.long 0
.long 0

.word 0xffff    // segment length, bits 0-15
.word 0x0       // segment base, bits 0-15
.byte 0x0       // segment base, bits 16-23
.byte 0b10011010 // flags (8 bits)
.byte 0b11001111 // flags (4 bits) + segment length, bits 16-19
.byte 0x0       // segment base, bits 24-31
gdt_end:

size =	. - _start
#if size+2 > 440
  #error "code is too large for boot sector"
#endif
. = . + (512 - size - 2)


	.byte	0x55, 0xAA		//2  byte boot signature
code_start:
 .incbin xstr(IMAGE)
code_end:
