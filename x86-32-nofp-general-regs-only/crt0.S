	.file	"crt0.cc"
	.text
	.section	.init,"ax",@progbits
	.p2align 4,,15
	.globl	_start
	.type	_start, @function
_start:
.LFB0:
	.cfi_startproc
	movl	$_ZL11gdt_pointer, %eax
#APP
# 83 "crt0.cc" 1
	lgdt (%eax) 
# 0 "" 2
# 90 "crt0.cc" 1
	jmp $0x08,$_pstart
# 0 "" 2
#NO_APP
	ud2
	.cfi_endproc
.LFE0:
	.size	_start, .-_start
	.p2align 4,,15
	.globl	_pstart
	.type	_pstart, @function
_pstart:
.LFB1:
	.cfi_startproc
#APP
# 104 "crt0.cc" 1
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	
# 0 "" 2
# 105 "crt0.cc" 1
	movl $__stack_end-4, %esp
# 0 "" 2
# 106 "crt0.cc" 1
	movl $__stack_end-4, %ebp
# 0 "" 2
# 107 "crt0.cc" 1
	push $0
# 0 "" 2
# 108 "crt0.cc" 1
	jmp _cstart
# 0 "" 2
#NO_APP
	ud2
	.cfi_endproc
.LFE1:
	.size	_pstart, .-_pstart
	.section	.rodata
	.align 4
	.type	_ZL11gdt_pointer, @object
	.size	_ZL11gdt_pointer, 6
_ZL11gdt_pointer:
	.value	23
	.long	_ZL9gdt_table
	.data
	.align 4
	.type	_ZL9gdt_table, @object
	.size	_ZL9gdt_table, 24
_ZL9gdt_table:
	.value	0
	.value	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.value	-1
	.value	0
	.byte	0
	.byte	-102
	.byte	-49
	.byte	0
	.value	-1
	.value	0
	.byte	0
	.byte	-110
	.byte	-49
	.byte	0
	.ident	"GCC: (Gentoo Hardened 8.2.0-r6 p1.7) 8.2.0"
	.section	.note.GNU-stack,"",@progbits
