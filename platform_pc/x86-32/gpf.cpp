/*
 * gpf.cpp
 *
 *  Created on: Aug 19, 2021
 *      Author: Korneliusz Osmenda
 */

#include "interrupts.h"
#include "gpf.hpp"
#include "vm86.hpp"

__attribute__((weak))
bool vm86_handle_gpf(Gpf_stack *)
{
	return false;
}

__attribute__((cdecl))
static void
gpfC (Gpf_stack *frame)
{
	if (frame->eflags & (1<<17))
		if (vm86_handle_gpf(frame))
			return;
	//gpf not handled
	asm("cli");
	while(1){
		asm("hlt");
	}
}

__attribute__ ((naked))
void gpf (struct interrupt_frame *)
{
	asm(
		".cfi_def_cfa_offset 8\n\t"
		"push %%eax\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"push %%ecx\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"push %%edx\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"push %%ebx\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"push %%ebp\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"push %%esi\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"push %%edi\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"movl %%esp, %%eax\n\t"
		//"sub $44, %%eax\n\t"
		"push %%eax\n\t"
		".cfi_adjust_cfa_offset 4\n\t"
		"call %P0\n\t"
		"addl $4, %%esp\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"pop %%edi\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"pop %%esi\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"pop %%ebp\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"pop %%ebx\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"pop %%edx\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"pop %%ecx\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"pop %%eax\n\t"
		".cfi_adjust_cfa_offset -4\n\t"
		"addl $4, %%esp\n\t"
		".cfi_def_cfa_offset 4\n\t"
		"iret\n\t"
			:: "i" (gpfC)
	);

}

