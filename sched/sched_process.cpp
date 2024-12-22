/*
 * sched_process.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"
#include "io.h"



void bezbios_sched_task_ready(ThreadControlBlock * tid, bool is_ready)
{
	tid->ready = is_ready;
}

ThreadControlBlock * rr_next_task()
{
	ThreadControlBlock * tid = bezbios_sched_get_tid();
	// rr no priority
	ENTER_ATOMIC();
	for(ThreadControlBlock * check = tid->next;check != tid;check = check->next)
	{
		if(check->ready)
		{
			EXIT_ATOMIC();
			return check;
		}
	}
	EXIT_ATOMIC();
	return &idle_tcb; // falltrough to bezbios_main task
}

int bezbios_sched_free_cpu(bool reschedule)
{
	ENTER_ATOMIC();
	ThreadControlBlock * tid = bezbios_sched_get_tid();
	ThreadControlBlock * wait_tid = rr_next_task();
	bezbios_sched_task_ready(wait_tid, 0);
	bezbios_sched_task_ready(tid,reschedule);
	bezbios_sched_switch_context(wait_tid);
	EXIT_ATOMIC();
	return 1;	
}

int bezbios_sched_sel_task(bool reschedule,ThreadControlBlock * sel_tid)
{
	ENTER_ATOMIC();
	ThreadControlBlock * tid = bezbios_sched_get_tid();
	bezbios_sched_task_ready(tid,reschedule);
	if (sel_tid != tid)
	{
		bezbios_sched_task_ready(sel_tid, 0);
		bezbios_sched_switch_context(sel_tid);
		EXIT_ATOMIC();
		return 1;
	}
	else
	{
		EXIT_ATOMIC();
	}
	return 0;
}

Exit_func * efunc_head = nullptr;

void bezbios_sched_exit_func_reg(Exit_func * efunc)
{
	if(efunc_head)
		efunc->plug(efunc_head,true);
	else
		efunc_head = efunc;
}


void bezbios_sched_exit(ThreadControlBlock * tid)
{
	ENTER_ATOMIC();
	bezbios_sched_task_ready(tid,0);
	bezbios_sched_destroy_task(tid);
	for(Exit_func* efunc=efunc_head;efunc != nullptr;efunc=efunc->next)
	{
		efunc->func(tid);
	}
	ThreadControlBlock * wait_tid = rr_next_task();
	bezbios_sched_switch_context(wait_tid);
	EXIT_ATOMIC();
	return;
}
