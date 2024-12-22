/*
 * sched.c
 *
 *  Created on: Nov 11, 2020
 *      Author: Korneliusz Osmenda
 */


#include <sched/bezbios_sched_api.h>
#include "sched_low.h"
#include "io.h"

#include "thread_py.h"

#include <uart/tlay2_dbgout.hpp>

static
__attribute__((used))
__attribute__((section(".debug_gdb_scripts")))
const char thread_py[] =
	"\x01" THREAD_PY_PATH "/thread.py";


ThreadControlBlock idle_tcb = {};

static ThreadControlBlock * current_tcb=nullptr;

void bezbios_sched_switch_context(ThreadControlBlock * nexttask)
{
	ENTER_ATOMIC();
	ThreadControlBlock* tid = current_tcb;
	current_tcb = nexttask;

	auto entry = current_tcb->entry;
	current_tcb->entry = nullptr;

	BezBios::Sched::m32ngro::
	switchcontext_int(&tid->stack,current_tcb->stack, entry,current_tcb->val);
	EXIT_ATOMIC();
}

ThreadControlBlock * bezbios_sched_get_tid()
{
	return current_tcb;
}

bool bezbios_sched_is_idle()
{
	return current_tcb == &idle_tcb || !idle_tcb.ready;
}

void bezbios_sched_idle_it_is()
{
	current_tcb = &idle_tcb;
}

void bezbios_sched_create_task(ThreadControlBlock * tid, void(*entry)(void*),void * stackbottom, void* val)
{
	{
		DbgOut<UartBlocking> sender;
		sender.str("TASK STACK AT ").hex((long)stackbottom).end();
	}
	if(!current_tcb)
		current_tcb = tid;
	else
		tid->plug(current_tcb, false);

	tid->entry = entry;
	tid->stack = stackbottom;
	tid->val = val;
}

void bezbios_sched_destroy_task(ThreadControlBlock * tid)
{
	tid->stack = nullptr;
}

