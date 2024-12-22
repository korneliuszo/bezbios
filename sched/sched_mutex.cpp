/*
 * sched_mutex.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: Korneliusz Osmenda
 */

#include "bezbios_sched_api.h"
#include "io.h"

void BezBios::Sched::Mutex::aquire() {
	{
		ENTER_ATOMIC();
		if(locked == bezbios_sched_get_tid())
		{
			lock_cnt++;
			EXIT_ATOMIC();
			return;
		}
		EXIT_ATOMIC();
	}
	while (locked) {
		{
			ENTER_ATOMIC();
			TCB_LIST wait = {&wthreads};
			wait.tcb = bezbios_sched_get_tid();
			bezbios_sched_task_ready(wait.tcb,0);
			EXIT_ATOMIC();
			bezbios_sched_switch_context (locked);
		}
	}
	ENTER_ATOMIC();
	locked = bezbios_sched_get_tid();
	lock_cnt++;
	EXIT_ATOMIC();
}
void BezBios::Sched::Mutex::release() {
	ENTER_ATOMIC();
	if(--lock_cnt)
	{
		EXIT_ATOMIC();
		return;
	}
	locked = nullptr;
	EXIT_ATOMIC();	
	if(wthreads.prev != &wthreads)
	{
		bezbios_sched_task_ready(wthreads.prev->tcb,1);
		wthreads.prev->unplug();
	}
}


void BezBios::Sched::Mutex::mutex_head_exit(ThreadControlBlock * tid)
{
	ENTER_ATOMIC();
	for (BezBios::Sched::Mutex * head=mutex_list_head;head!=nullptr;head=head->next)
	{
		for(TCB_LIST * check = head->wthreads.next;check != &head->wthreads;check = check->next)
		{
			if(check->tcb == tid)
				check->unplug();
		}
	}
	EXIT_ATOMIC();
}


BezBios::Sched::Mutex::Mutex()
{
	ENTER_ATOMIC();
	next=mutex_list_head;
	mutex_list_head=this;
	lock_cnt = 0;
	efunc.init(mutex_head_exit);
	EXIT_ATOMIC();

}

void BezBios::Sched::ConditionVariable::wait()
{
	ENTER_ATOMIC();	
	TCB_LIST wait = {&wthreads};
	wait.tcb = bezbios_sched_get_tid();
	EXIT_ATOMIC();	
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
	ENTER_ATOMIC();	
	wait_tid = bezbios_sched_get_tid();
	bezbios_sched_free_cpu(0);
	EXIT_ATOMIC();	
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

 void BezBios::Sched::ConditionVariable::cv_head_exit(ThreadControlBlock * tid)
{
	ENTER_ATOMIC();
	for (BezBios::Sched::ConditionVariable * head=condition_variable_list_head;head!=nullptr;head=head->next)
		for(TCB_LIST * check = head->wthreads.next;check != &head->wthreads;check = check->next)
		{
			if(check->tcb == tid)
				check->unplug();
		}

	EXIT_ATOMIC();
}

BezBios::Sched::ConditionVariable::ConditionVariable()
{
	ENTER_ATOMIC();
	next=condition_variable_list_head;
	condition_variable_list_head=this;
	efunc.init(cv_head_exit);
	EXIT_ATOMIC();

}


