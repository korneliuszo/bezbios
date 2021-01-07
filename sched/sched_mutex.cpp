/*
 * sched_mutex.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"
#include "io.h"

void BezBios::Sched::Mutex::aquire() {
	int tid = bezbios_sched_get_tid();
	asm("cli");
	while (locked) {
		waiting.set(tid, 1);
		bezbios_sched_task_ready(tid,0);
		bezbios_sched_switch_context (locked);
		asm("cli");
	}
	locked = tid;
	waiting.set(tid, 0);
	asm("sti");
}
void BezBios::Sched::Mutex::release() {
	locked = 0;
	int tid = bezbios_sched_get_tid();
	int wait_tid = 0; // falltrough to bezbios_main task
	for(int it=tid;it < CONFIG_MAX_THREADS;it++)
	{
		if(!wait_tid && waiting.get(it))
			wait_tid = it;
	}
	for(int it=0;it < tid;it++)
	{
		if(!wait_tid && waiting.get(it))
			wait_tid = it;
	}
	if (wait_tid)
	{
		bezbios_sched_task_ready(wait_tid,1);
	}
}

void BezBios::Sched::ConditionVariable::wait()
{
	int tid = bezbios_sched_get_tid();
	asm volatile("cli");
	waiting.set(tid,1);
	bezbios_sched_free_cpu(0);
}

bool BezBios::Sched::ConditionVariable::notify_all()
{
	bool resheduleable = false;
	ENTER_ATOMIC();
	for(int it=0;it < CONFIG_MAX_THREADS;it++)
	{
		if(waiting.get(it))
		{
			bezbios_sched_task_ready(it,1);
			waiting.set(it,0);
			resheduleable = true;
		}
	}
	EXIT_ATOMIC();
	return resheduleable;
}


