/*
 * sched_asm.cc
 *
 *  Created on: Nov 15, 2020
 *      Author: Korneliusz Osmenda
 */

#include "sched_low.h"

void
BezBios::Sched::m32ngro::
switchcontext_int(void **prev_s, void *next_s, void (*entry_s)(void *),void * val_s) {
	asm goto (
			"movl %%esp, (%0)\n\t"
			"switchcontext_thread_pc:\n\t"
			"movl %1, %%esp\n\t"
			"jecxz %l5\n\t"
			"push %4\n\t" // arg
			"push %3\n\t" // return to NULL
			"sti\n\t"
			"jmp %2\n\t"
			: : "r" (prev_s), "r" (next_s), "ecx"(entry_s), "i"(nullptr), "r" (val_s)
			: "esi","edi","ebp" : no_entry );
no_entry:
	asm("sti");
}
