/*
 * vm86.hpp
 *
 *  Created on: Aug 17, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef PLATFORM_PC_X86_32_VM86_HPP_
#define PLATFORM_PC_X86_32_VM86_HPP_

#include "gpf.hpp"

struct Vmm86Regs
{
	unsigned long ax;
	unsigned long bx;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long bp;
	unsigned long eflags;
};

struct Vmm86SegmentRegisters {
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;
};

struct LONGADDR
{
	unsigned long segment;
	unsigned long offset;
};

LONGADDR vmm86_to_segment(void * ptr, bool codesection = false);
bool vm86_handle_gpf(Error_stack *frame);
void callx86int(unsigned char isr, const Vmm86Regs * in, Vmm86Regs * out, Vmm86SegmentRegisters *seg);


#endif /* PLATFORM_PC_X86_32_VM86_HPP_ */
