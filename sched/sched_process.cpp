/*
 * sched_process.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"

static int threads_wfi[CONFIG_MAX_THREADS];
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
	int int_tid = 0;
	for(long it=0;it < CONFIG_MAX_THREADS ;it++)
	{
		if(!int_tid &&threads_wfi[it] == interrupt +2)
			int_tid = it;
	}
	if (int_tid)
	{
		threads_wfi[int_tid] = 0;
		if (int_tid != tid)
		{
			threads_wfi[tid] = 1;
			bezbios_sched_switch_context(int_tid);
		}
	}
}

int bezbios_sched_free_cpu()
{
	int tid = bezbios_sched_get_tid();
	int wait_tid = 0; // falltrough to bezbios_main task
	for(long it=tid;it < CONFIG_MAX_THREADS;it++)
	{
		if(!wait_tid && threads_wfi[it] == 1)
			wait_tid = it;
	}
	for(auto it=0;it < tid;it++)
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
