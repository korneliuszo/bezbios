/*
 * e820.cpp
 *
 *  Created on: 20 gru 2022
 *      Author: nyuu
 */
#include "e820.hpp"
#include "vm86.hpp"
#include "string.h"

__attribute__((section(".loram_scratch")))
static unsigned char vm86_scratch[30];

e820_ret e820_call(uint32_t cont)
{
	Vmm86Regs in;
	Vmm86SegmentRegisters seg;
	Vmm86Regs out;

	in.ax = 0xe820;
	in.bx = cont;
	LONGADDR scratch_long = vmm86_to_segment(vm86_scratch);
	seg.es = scratch_long.segment;
	in.di = scratch_long.offset;
	in.cx = 30;
	in.dx = 0x534D4150;

	callx86int(0x15,&in,&out,&seg);
	e820_ret ret;
	if(out.ax != 0x534D4150)
	{
		ret.success = false;
		return ret;
	}
	ret.success = true;
	memcpy(&ret.data,vm86_scratch,sizeof(ret.data));
	ret.cont = out.bx;
	return ret;
}
