/*
 * int3.cpp
 *
 *  Created on: Jun 7, 2023
 *      Author: Korneliusz Osmenda
 */

#include "interrupts.h"
#include <uart/tlay2_dbgout.hpp>

void int3isr(Isr_stack *stack)
{
	DbgOut<UartBlocking> sender;
	sender.str("INT3!!!").end();
	sender.str("EIP: ").hex(stack->eip).end();
	sender.str("CS: ").hex(stack->cs).end();
	sender.str("EFLAGS: ").hex(stack->eflags).end();
	sender.str("EAX: ").hex(stack->eax).end();
	sender.str("EBX: ").hex(stack->ebx).end();
	sender.str("ECX: ").hex(stack->ecx).end();
	sender.str("EDX: ").hex(stack->edx).end();
	sender.str("ESI: ").hex(stack->esi).end();
	sender.str("EDI: ").hex(stack->edi).end();
	sender.str("EBP: ").hex(stack->ebp).end();
	sender.str("DS: ").hex(stack->ds).end();
	sender.str("ES: ").hex(stack->es).end();
	sender.str("FS: ").hex(stack->fs).end();
	sender.str("GS: ").hex(stack->gs).end();
}

