/*
 * sched_process.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"

static volatile int threads_wfi[CONFIG_MAX_THREADS];
// 0 - don't schedule
// 1 - wait for free cpu
// +2 - wait for interrupt


void bezbios_sched_task_ready(int tid, int is_ready)
{
	if(is_ready)
		threads_wfi[tid] = 1;
	else
		threads_wfi[tid] = 0;
}
void bezbios_sched_wfi(int interrupt)
{
	int tid = bezbios_sched_get_tid();
	threads_wfi[tid] = interrupt + 2;
	bezbios_sched_free_cpu();
}
void bezbios_sched_interrupt_handled(int interrupt)
{
	int tid = bezbios_sched_get_tid();
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
		threads_wfi[tid] = 1;
		bezbios_sched_free_cpu();
	}
}

int bezbios_sched_free_cpu()
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
		bezbios_sched_switch_context(wait_tid);
		return 1;
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
