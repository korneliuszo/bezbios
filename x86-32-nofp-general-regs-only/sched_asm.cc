/*
 * sched_asm.cc
 *
 *  Created on: Nov 15, 2020
 *      Author: Korneliusz Osmenda
 */

#include "sched_low.h"

void
BezBios::Sched::m32ngro::
switchcontext_int(void **prev_s, void *next_s, void (*entry_s)()) {
	asm goto (
			"movl %%esp, (%0)\n\t"
			"movl %1, %%esp\n\t"
			"jecxz %l4\n\t"
			"push %3\n\t" // we want GPF
			"sti\n\t"
			"jmp %2\n\t"
			: : "r" (prev_s), "r" (next_s), "ecx"(entry_s), "i"(nullptr)
			: "ebx","esi","edi","ebp" : no_entry );
no_entry:
	asm("sti");
}
