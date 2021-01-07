/*
 * sched_process.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"

static volatile  BezBios::Sched::Bitfield<CONFIG_MAX_THREADS> threads_wfi;



void bezbios_sched_task_ready(int tid, bool is_ready)
{
		threads_wfi.set(tid, is_ready);
}


int bezbios_sched_free_cpu(bool reschedule)
{
	asm("cli");
	int tid = bezbios_sched_get_tid();
	int wait_tid = 0; // falltrough to bezbios_main task
	// rr no priority
	for(int it=tid+1;it < CONFIG_MAX_THREADS;it++)
	{
		if(!wait_tid && threads_wfi.get(it))
			wait_tid = it;
	}
	for(int it=0;it < tid+1;it++)
	{
		if(!wait_tid && threads_wfi.get(it))
			wait_tid = it;
	}
	if (wait_tid != tid)
	{
		bezbios_sched_task_ready(wait_tid, 0);
		bezbios_sched_task_ready(tid,reschedule);
		bezbios_sched_switch_context(wait_tid);
		return 1;
	}
	else
	{
		asm("sti");
	}
	return 0;
}

int bezbios_sched_free_cpu_exit()
{
	int tid = bezbios_sched_get_tid();
	int wait_tid = 0; // falltrough to bezbios_main task
	// rr no priority
	for(int it=tid+1;it < CONFIG_MAX_THREADS;it++)
	{
		if(!wait_tid && threads_wfi.get(it))
			wait_tid = it;
	}
	for(int it=0;it < tid+1;it++)
	{
		if(!wait_tid && threads_wfi.get(it))
			wait_tid = it;
	}
	if (wait_tid != tid)
	{
		bezbios_sched_task_ready(wait_tid, 0);
		bezbios_sched_switch_context_exit(wait_tid);
		return 1;
	}
	return 0;
}
