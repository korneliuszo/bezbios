/*
 * sched_process.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"

static volatile int threads_wfi[CONFIG_MAX_THREADS];
// 0 - don't schedule/running
// 1 - wait for free cpu
// +2 - wait for interrupt


void bezbios_sched_task_ready(int tid, int is_ready)
{
		threads_wfi[tid] = is_ready;
}
void bezbios_sched_wfi(int interrupt)
{
	bezbios_sched_free_cpu(interrupt + 2);
}
void bezbios_sched_interrupt_handled(int interrupt)
{
	bool int_schedule = false;
	for(int it=0;it < CONFIG_MAX_THREADS ;it++)
	{
		if(threads_wfi[it] == interrupt +2)
		{
			threads_wfi[it] = 1;
			int_schedule = true;
		}
	}
	if (int_schedule)
	{
		bezbios_sched_free_cpu(1);
	}
}

int bezbios_sched_free_cpu(int reschedule)
{
	asm("cli");
	int tid = bezbios_sched_get_tid();
	int wait_tid = 0; // falltrough to bezbios_main task
	// rr no priority
	for(int it=tid+1;it < CONFIG_MAX_THREADS;it++)
	{
		if(!wait_tid && threads_wfi[it] == 1)
			wait_tid = it;
	}
	for(int it=0;it < tid+1;it++)
	{
		if(!wait_tid && threads_wfi[it] == 1)
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
		if(!wait_tid && threads_wfi[it] == 1)
			wait_tid = it;
	}
	for(int it=0;it < tid+1;it++)
	{
		if(!wait_tid && threads_wfi[it] == 1)
			wait_tid = it;
	}
	threads_wfi[wait_tid] = 0;
	if (wait_tid != tid)
	{
		bezbios_sched_switch_context_exit(wait_tid);
		return 1;
	}
	return 0;
}
