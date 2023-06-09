/*
 * int3.cpp
 *
 *  Created on: Jun 7, 2023
 *      Author: Korneliusz Osmenda
 */

#include "interrupts.h"
#include "dbg.hpp"
#include <uart/tlay2_dbgout.hpp>
#include <stdint.h>

void int1isr(Isr_stack *stack)
{
	uint32_t dr6, dr7;
	asm("mov %%DR6, %0": "=r"(dr6));

	DbgOut<UartBlocking> sender;
	sender.str("DBG!!!").end();
	sender.str("DR6: ").hex(dr6).end();
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
	asm("mov %%DR7, %0": "=r"(dr7));
	dr7&=~((0x3<<0));
	asm("mov %0, %%DR7":: "r"(dr7));
}

void install_bp1(void* addr)
{
	asm("mov %0, %%DR0":: "r"(addr));

	uint32_t dr7;
	asm("mov %%DR7, %0": "=r"(dr7));
	dr7&=~((0xf<<16));
	dr7|=0x3 | (0x3<<8) | (0x0<<16);
	asm("mov %0, %%DR7":: "r"(dr7));
}
