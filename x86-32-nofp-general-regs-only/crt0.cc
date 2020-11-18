/*
 * crt0.cc
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

extern "C" {

extern int __stack_end;

void
__attribute__((used))
__attribute__((naked))
__attribute__((section(".init"))) _start(void);
}


void
__attribute__((used))
__attribute__((naked))
__attribute__((section(".init"))) _start(void) {
	asm volatile("movl %0, %%esp" : : "i"(&__stack_end));
	asm volatile("movl %0, %%ebp" : : "i"(&__stack_end));
	asm volatile("jmp _cstart");
}
