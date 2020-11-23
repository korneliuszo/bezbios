/*
 * sched_process.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"

#include <etl/array.h>

static etl::array<int,CONFIG_MAX_THREADS> threads_wfi;
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
	for(auto it=threads_wfi.begin();it != threads_wfi.end();it++)
	{
		if(!int_tid && *it == interrupt +2)
			int_tid = it - threads_wfi.begin();
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
	for(auto it=threads_wfi.begin()+tid;it != threads_wfi.end();it++)
	{
		if(!wait_tid && *it == 1)
			wait_tid = it - threads_wfi.begin();
	}
	for(auto it=threads_wfi.begin();it != threads_wfi.begin()+tid;it++)
	{
		if(!wait_tid && *it == 1)
			wait_tid = it - threads_wfi.begin();
	}
	threads_wfi[wait_tid] = 0;
	if (wait_tid != tid)
	{
		bezbios_sched_switch_context(wait_tid);
		return 1;
	}
	return 0;
}
