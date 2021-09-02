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
	ENTER_ATOMIC();
	while (locked) {
		waiting.set(tid, 1);
		bezbios_sched_task_ready(tid,0);
		bezbios_sched_switch_context (locked);
		asm("cli");
	}
	locked = tid;
	waiting.set(tid, 0);
	EXIT_ATOMIC();
}
void BezBios::Sched::Mutex::release() {
	ENTER_ATOMIC();
	locked = 0;
	int tid = bezbios_sched_get_tid();
	int wait_tid = 0; // falltrough to bezbios_main task
	for(int it=tid;it < CONFIG_MAX_THREADS;it++)
	{
		if(!wait_tid && waiting.get(it))
			wait_tid = it;
	}
	for(int it=1;it < tid;it++)
	{
		if(!wait_tid && waiting.get(it))
			wait_tid = it;
	}
	if (wait_tid)
	{
		waiting.set(wait_tid,0);
		bezbios_sched_task_ready(wait_tid,1);
	}
	EXIT_ATOMIC();
}

void BezBios::Sched::Mutex::destroy_task(int tid)
{
	ENTER_ATOMIC();
	for (Mutex * head=this;head!=nullptr;head=head->next)
		head->waiting.set(tid,0);
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

void BezBios::Sched::ConditionVariable::destroy_task(int tid)
{
	ENTER_ATOMIC();
	for (ConditionVariable * head=this;head!=nullptr;head=head->next)
		head->waiting.set(tid,0);
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


