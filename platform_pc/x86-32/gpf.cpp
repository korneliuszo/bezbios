/*
 * gpf.cpp
 *
 *  Created on: Aug 19, 2021
 *      Author: Korneliusz Osmenda
 */

#include "interrupts.h"
#include "gpf.hpp"
#include "vm86.hpp"
#include <uart/tlay2_dbgout.hpp>

__attribute__((weak))
bool vm86_handle_gpf(Error_stack *)
{
	return false;
}

void
gpfC (Error_stack *frame)
{
	if (frame->eflags & (1<<17))
		if (vm86_handle_gpf(frame))
			return;
	//gpf not handled
	asm("cli");
	{
		DbgOut<UartBlocking> sender;
		sender.str("OOPS!! unhandled GPF AT").hex(frame->eip).end();
		unsigned long * stack = (unsigned long*)frame;
		for(int i=0;i<64;i++)
		{
			sender.hex(i).str(" ").hex(stack[i]).end();
		}
	}
	while(1){
		asm("hlt");
	}
}


