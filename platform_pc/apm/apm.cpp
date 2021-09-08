/*
 * apm.cpp
 *
 *  Created on: Sep 7, 2021
 *      Author: Korneliusz Osmenda
 */

#include "apm.hpp"
#include "vm86.hpp"
#include "tss.h"

APM::APM()
{
	Vmm86Regs in = {};
	Vmm86Regs out;
	Vmm86SegmentRegisters seg = {};
	in.ax = 0x5304;
	callx86int(0x15,&in,&out,&seg);
	in.ax = 0x5303;
	callx86int(0x15,&in,&out,&seg);
	apm_setup_gdt(out.ax<<4,out.cx<<4,out.dx<<4);
	offset=out.bx;
}

bool APM::call(unsigned short a, unsigned short b, unsigned short c)
{
	bool ret;
	struct __attribute__((packed)) {
		unsigned long address;
		unsigned short segment;
	} bios32_indirect = { offset, 0x20 };
	asm volatile (
			"lcall *(%[ptr])\n\t"
	: "+ax" (a), "+bx"(b), "+cx"(c), "=@ccc"(ret)
	: [ptr]"D"(&bios32_indirect) : "memory"
	);
	return ret;
}

void APM::shutdown(void)
{
	call(0x530e,0,0x0101);
	call(0x5308,1,1);
	call(0x5307,1,3);
}

APM apm;
