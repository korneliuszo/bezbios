	.file	"crt0.cc"
	.text
	.section	.init,"ax",@progbits
	.p2align 4,,15
	.globl	_start
	.type	_start, @function
_start:
.LFB0:
	.cfi_startproc
#APP
# 23 "crt0.cc" 1
	movl $__stack_end, %esp
# 0 "" 2
# 24 "crt0.cc" 1
	movl $__stack_end, %ebp
# 0 "" 2
# 25 "crt0.cc" 1
	jmp _cstart
# 0 "" 2
#NO_APP
	ud2
	.cfi_endproc
.LFE0:
	.size	_start, .-_start
	.ident	"GCC: (Gentoo Hardened 8.2.0-r6 p1.7) 8.2.0"
	.section	.note.GNU-stack,"",@progbits
