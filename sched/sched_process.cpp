/*
 * sched_process.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"
#include "io.h"

static volatile  BezBios::Sched::Bitfield<CONFIG_MAX_THREADS> threads_wfi;



void bezbios_sched_task_ready(int tid, bool is_ready)
{
		threads_wfi.set(tid, is_ready);
}

int rr_next_task()
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
	return wait_tid;
}

int bezbios_sched_free_cpu(bool reschedule)
{
	ENTER_ATOMIC();
	int tid = bezbios_sched_get_tid();
	bezbios_sched_task_ready(tid,reschedule);
	int wait_tid = rr_next_task();
	if (wait_tid != tid)
	{
		bezbios_sched_task_ready(wait_tid, 0);
		bezbios_sched_switch_context(wait_tid);
		return 1;
	}
	else
	{
		EXIT_ATOMIC();
	}
	return 0;
}

void bezbios_sched_exit(int tid)
{
	asm("cli");
	bezbios_sched_task_ready(tid,0);
	BezBios::Sched::mutex_list_head->destroy_task(tid);
	BezBios::Sched::condition_variable_list_head->destroy_task(tid);
	int wait_tid = rr_next_task();
	bezbios_sched_switch_context(wait_tid);
	return;
}
