/*
 * sched_asm.cc
 *
 *  Created on: Nov 15, 2020
 *      Author: Korneliusz Osmenda
 */

#include "sched_low.h"

void
BezBios::Sched::m32ngro::
switchcontext_int(void * volatile *prev_s, void *next_s, void (*entry_s)(void *),void * val_s) {
	asm (
			"pushf\n\t"
			"test %0, %0\n\t"
			"jz no_save\n\t"
			"movl %%esp, (%0)\n\t"
			"no_save:\n\t"
			"switchcontext_thread_pc:\n\t"
			"movl %1, %%esp\n\t"
			"jecxz no_entry\n\t"
			"push %4\n\t" // arg
			"push %3\n\t" // return to NULL
			"sti\n\t"
			"jmp *%2\n\t" // call from NULL
			"no_entry:\n\t"
			"popf\n\t"
			: : "eax" (prev_s), "ebx" (next_s), "ecx"(entry_s), "i"(nullptr), "edx" (val_s)
			: "esi","edi","ebp", "memory" );
}
