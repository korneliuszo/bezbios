/*
 * sched_mutex.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"
#include "io.h"

void BezBios::Sched::Mutex::aquire() {
	ENTER_ATOMIC();
	while (locked) {
		{
			TCB_LIST wait = {&wthreads};
			wait.tcb = bezbios_sched_get_tid();
			bezbios_sched_task_ready(wait.tcb,0);
			bezbios_sched_switch_context (locked);
		}
		asm("cli":::"memory");
	}
	locked = bezbios_sched_get_tid();
	EXIT_ATOMIC();
}
void BezBios::Sched::Mutex::release() {
	ENTER_ATOMIC();
	locked = nullptr;
	if(wthreads.prev != &wthreads)
	{
		bezbios_sched_task_ready(wthreads.prev->tcb,1);
		wthreads.prev->unplug();
	}
	EXIT_ATOMIC();
}

void BezBios::Sched::Mutex::destroy_task(ThreadControlBlock * tid)
{
	ENTER_ATOMIC();
	for (Mutex * head=this;head!=nullptr;head=head->next)
	{
		for(TCB_LIST * check = head->wthreads.next;check != &head->wthreads;check = check->next)
		{
			if(check->tcb == tid)
				check->unplug();
		}
	}
	EXIT_ATOMIC();
}

BezBios::Sched::Mutex * BezBios::Sched::mutex_list_head;

BezBios::Sched::Mutex::Mutex()
{
	ENTER_ATOMIC();
	next=BezBios::Sched::mutex_list_head;
	BezBios::Sched::mutex_list_head=this;
	EXIT_ATOMIC();

}

void BezBios::Sched::ConditionVariable::wait()
{
	asm volatile("cli":::"memory");
	TCB_LIST wait = {&wthreads};
	wait.tcb = bezbios_sched_get_tid();
	bezbios_sched_free_cpu(0);
	(void)wait;
}

bool BezBios::Sched::ConditionVariable::notify_all()
{
	bool resheduleable = false;
	ENTER_ATOMIC();
	for(TCB_LIST * check = wthreads.next;check != &wthreads;check = check->next)
	{
			bezbios_sched_task_ready(check->tcb,1);
			check->unplug();
			resheduleable = true;
	}

	EXIT_ATOMIC();
	return resheduleable;
}

void BezBios::Sched::ConditionVariableSingle::wait()
{
	asm volatile("cli":::"memory");
	wait_tid = bezbios_sched_get_tid();
	bezbios_sched_free_cpu(0);
}

ThreadControlBlock * BezBios::Sched::ConditionVariableSingle::notify()
{
	ThreadControlBlock * resheduleable = nullptr;
	ENTER_ATOMIC();
	resheduleable = wait_tid;
	if (resheduleable)
	{
		bezbios_sched_task_ready(resheduleable,1);
		wait_tid = nullptr;
	}
	EXIT_ATOMIC();
	return resheduleable;
}

void BezBios::Sched::ConditionVariable::destroy_task(ThreadControlBlock * tid)
{
	ENTER_ATOMIC();
	for (ConditionVariable * head=this;head!=nullptr;head=head->next)
		for(TCB_LIST * check = head->wthreads.next;check != &head->wthreads;check = check->next)
		{
			if(check->tcb == tid)
				check->unplug();
		}

	EXIT_ATOMIC();
}

BezBios::Sched::ConditionVariable * BezBios::Sched::condition_variable_list_head;

BezBios::Sched::ConditionVariable::ConditionVariable()
{
	ENTER_ATOMIC();
	next=BezBios::Sched::condition_variable_list_head;
	BezBios::Sched::condition_variable_list_head=this;
	EXIT_ATOMIC();

}


