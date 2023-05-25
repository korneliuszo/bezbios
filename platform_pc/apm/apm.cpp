/*
 * apm.cpp
 *
 *  Created on: Sep 7, 2021
 *      Author: Korneliusz Osmenda
 */

#include "apm.hpp"
#include "vm86.hpp"
#include "tss.h"
#include "io.h"

#include <uart/tlay2_dbgout.hpp>


APM::APM()
{
	working = false;
	Vmm86Regs in = {};
	Vmm86Regs out;
	Vmm86SegmentRegisters seg = {};
	in.ax = 0x5303;
	in.bx = 0;
	callx86int(0x15,&in,&out,&seg);
	if(out.eflags&(1<<0))
		return;
	apm_setup_gdt(out.ax<<4,out.cx<<4,out.dx<<4);
	offset=out.bx;
	call(0x530e,0,0x0101);
	working = true;
}

bool APM::call(unsigned short a, unsigned short b, unsigned short c)
{
	bool ret;
	ENTER_ATOMIC();
	static struct __attribute__((packed)) {
		unsigned long address;
		unsigned short segment;
	} bios32_indirect = { 0, 0x40 };
	bios32_indirect.address = offset;
	asm volatile (
			"lcall *(%p[ptr])\n\t"
	:
	  "=@ccc"(ret)
	: "ax"(a),
	  "bx"(b),
	  "cx"(c),
	  [ptr]"i"(&bios32_indirect) : "memory"
	);
	EXIT_ATOMIC();
	return ret;
}

void APM::shutdown(void)
{
	if(!working)
		return;
	call(0x5308,1,1);
	call(0x5307,1,3);
}

APM apm;
