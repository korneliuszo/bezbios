/*
 * gpf.hpp
 *
 *  Created on: Aug 19, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef PLATFORM_PC_X86_32_GPF_HPP_
#define PLATFORM_PC_X86_32_GPF_HPP_


struct __attribute__((packed)) Gpf_stack {
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	unsigned long error_code;
	unsigned long eip;
	unsigned long cs;
	unsigned long eflags;
};
static_assert(sizeof(Gpf_stack) == 44, "Verifying size failed!");

template<typename LOSTACK>
struct __attribute__((packed)) User_stack : LOSTACK {
	unsigned long esp;
	unsigned long ss;
};

static_assert(sizeof(User_stack<Gpf_stack>) == 52, "Verifying size failed!");


#endif /* PLATFORM_PC_X86_32_GPF_HPP_ */
