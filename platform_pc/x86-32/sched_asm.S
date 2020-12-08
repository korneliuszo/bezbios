	.file	"sched_asm.cc"
	.text
	.p2align 4,,15
	.globl	_ZN7BezBios5Sched7m32ngro17switchcontext_intEPPvS2_PFvS2_ES2_
	.type	_ZN7BezBios5Sched7m32ngro17switchcontext_intEPPvS2_PFvS2_ES2_, @function
_ZN7BezBios5Sched7m32ngro17switchcontext_intEPPvS2_PFvS2_ES2_:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	movl	20(%esp), %eax
	movl	24(%esp), %edx
	movl	28(%esp), %ecx
	movl	32(%esp), %ebx
#APP
# 23 "sched_asm.cc" 1
	movl %esp, (%eax)
	switchcontext_thread_pc:
	movl %edx, %esp
	jecxz .L2
	push %ebx
	push $0
	sti
	jmp %ecx
	
# 0 "" 2
#NO_APP
.L2:
.L3:
#APP
# 25 "sched_asm.cc" 1
	sti
# 0 "" 2
#NO_APP
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 16
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 12
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 8
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE0:
	.size	_ZN7BezBios5Sched7m32ngro17switchcontext_intEPPvS2_PFvS2_ES2_, .-_ZN7BezBios5Sched7m32ngro17switchcontext_intEPPvS2_PFvS2_ES2_
	.ident	"GCC: (Gentoo Hardened 8.2.0-r6 p1.7) 8.2.0"
	.section	.note.GNU-stack,"",@progbits
