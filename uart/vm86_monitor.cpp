/*
 * vm86_monitor.cpp
 *
 *  Created on: Aug 23, 2021
 *      Author: Korneliusz Osmenda
 */

#include "tlay2.hpp"
#include "io.h"
#include "endianbuff.h"
#include "vm86_monitor.hpp"
#include "vm86.hpp"

__attribute__((section(".loram_scratch")))
static unsigned char vm86_page[64*1024];

void tlay2_v86_monitor(Tlay2 & tlay2)
{
	switch(tlay2.payload[0])
	{
	case 0x00: // get scratchaddr
	{
		LONGADDR addr = vmm86_to_segment(vm86_page);
		unsigned char buff[8];
		put_long_le(&buff[0],(unsigned long)vm86_page);
		put_short_le(&buff[4],addr.segment);
		put_short_le(&buff[6],addr.offset);
		tlay2.reply(buff,8);
		break;
	}
	case 0x01:
		if (tlay2.len == 24)
		{
			Vmm86Regs in;
			Vmm86Regs out;
			Vmm86SegmentRegisters seg;
			unsigned char irq;
			irq =tlay2.payload[1];
			in.ax = get_short_le(&tlay2.payload[2]);
			in.bx = get_short_le(&tlay2.payload[4]);
			in.cx = get_short_le(&tlay2.payload[6]);
			in.dx = get_short_le(&tlay2.payload[8]);
			in.si = get_short_le(&tlay2.payload[10]);
			in.di = get_short_le(&tlay2.payload[12]);
			in.bp = get_short_le(&tlay2.payload[14]);
			seg.ds = get_short_le(&tlay2.payload[16]);
			seg.es = get_short_le(&tlay2.payload[18]);
			seg.fs = get_short_le(&tlay2.payload[20]);
			seg.gs = get_short_le(&tlay2.payload[22]);
			callx86int(irq,&in,&out,&seg);
			unsigned char buff[16];
			put_short_le(&buff[0],out.ax);
			put_short_le(&buff[2],out.bx);
			put_short_le(&buff[4],out.cx);
			put_short_le(&buff[6],out.dx);
			put_short_le(&buff[8],out.si);
			put_short_le(&buff[10],out.di);
			put_short_le(&buff[12],out.bp);
			put_short_le(&buff[14],out.eflags);
			tlay2.reply(buff,16);
		}
	}
}
