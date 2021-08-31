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

__attribute__((cdecl))
void
gpfC (Error_stack *frame)
{
	if (frame->eflags & (1<<17))
		if (vm86_handle_gpf(frame))
			return;
	//gpf not handled
	asm("cli");
	while(1){
		asm("hlt");
	}
}


