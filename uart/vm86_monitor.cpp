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
		if (tlay2.len == 46)
		{
			Vmm86Regs in;
			Vmm86Regs out;
			Vmm86SegmentRegisters seg;
			unsigned char irq;
			irq =tlay2.payload[1];
			in.ax = get_long_le(&tlay2.payload[2]);
			in.bx = get_long_le(&tlay2.payload[6]);
			in.cx = get_long_le(&tlay2.payload[10]);
			in.dx = get_long_le(&tlay2.payload[14]);
			in.si = get_long_le(&tlay2.payload[18]);
			in.di = get_long_le(&tlay2.payload[22]);
			in.bp = get_long_le(&tlay2.payload[26]);
			seg.ds = get_long_le(&tlay2.payload[30]);
			seg.es = get_long_le(&tlay2.payload[34]);
			seg.fs = get_long_le(&tlay2.payload[38]);
			seg.gs = get_long_le(&tlay2.payload[42]);
			callx86int(irq,&in,&out,&seg);
			unsigned char buff[32];
			put_long_le(&buff[0],out.ax);
			put_long_le(&buff[4],out.bx);
			put_long_le(&buff[8],out.cx);
			put_long_le(&buff[12],out.dx);
			put_long_le(&buff[16],out.si);
			put_long_le(&buff[20],out.di);
			put_long_le(&buff[24],out.bp);
			put_long_le(&buff[28],out.eflags);
			tlay2.reply(buff,32);
		}
	}
}
