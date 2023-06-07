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

Memory_pool e820_scan_hole(uint32_t minaddr,uint32_t maxaddr)
{
	Memory_pool ret={};

	uint32_t cont = 0;
	uint32_t startaddr= 0;
	uint32_t len = 0;

	e820_ret r = e820_call(cont);
	if(!r.success)
		return ret;
	for(;r.cont;r = e820_call(r.cont)){
		uint32_t cur_start =  r.data.base + r.data.len;
		uint32_t cur_end = UINT32_MAX; // closed region
		e820_ret r_cont = e820_call(0);
		for(uint32_t r_last = r_cont.cont ;
				r_last !=0;
				r_cont = e820_call(r_cont.cont))
		{
			if(r_cont.data.base > cur_start)
			{
				if(r_cont.data.base <= cur_end)
					cur_end = r_cont.data.base - 1;
			}
			r_last = r_cont.cont;
		}
		if(cur_start < minaddr)
		{
			cur_start = minaddr;
		}
		if(cur_end > maxaddr)
		{
			cur_end = maxaddr;
		}
		uint32_t cur_len = cur_end + 1 - cur_start;
		if (cur_end > cur_start && cur_len > len)
		{
			startaddr = cur_start;
			len = cur_len;
		}
	}
	ret.addr = (void*)startaddr;
	ret.len = len;
	return ret;
}
