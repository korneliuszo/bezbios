/*
 * bezbios_sched_api.h
 *
 *  Created on: Nov 11, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef SCHED_BEZBIOS_SCHED_API_H_
#define SCHED_BEZBIOS_SCHED_API_H_

#include "list.hpp"
struct Thread_sp {
	void * stack;
	void (*entry)(void *);
	void * val;
};

class ThreadControlBlock :public Thread_sp, public List<ThreadControlBlock> {
public:
	bool ready;
	ThreadControlBlock() : List(true){};
	ThreadControlBlock(ThreadControlBlock * block) : List(block,true){};

};


void bezbios_sched_switch_context(ThreadControlBlock * nexttask);
ThreadControlBlock * bezbios_sched_get_tid();
bool bezbios_sched_is_idle();
void bezbios_sched_idle_it_is();
void bezbios_sched_create_task(ThreadControlBlock * tid, void (*entry)(void *), void *stackbottom, void * val);
void bezbios_sched_destroy_task(ThreadControlBlock * tid);


extern ThreadControlBlock idle_tcb;

void bezbios_sched_task_ready(ThreadControlBlock * tid, bool is_ready);
int bezbios_sched_free_cpu(bool reschedule);
int bezbios_sched_sel_task(bool reschedule,ThreadControlBlock * sel_tid);
ThreadControlBlock * rr_next_task();
void bezbios_sched_exit(ThreadControlBlock * tid);

template<void(*fn)(void), long size>
class BEZBIOS_CREATE_PROCESS
{
private:
	int stack[size] = {};
	ThreadControlBlock tcb;
	static void entry(void *)	 {
		fn();
		bezbios_sched_exit(bezbios_sched_get_tid());
	}
public:
	BEZBIOS_CREATE_PROCESS()
	{
		bezbios_sched_create_task(&tcb,entry, &stack[size-1], this);
		bezbios_sched_task_ready(&tcb,1);
	}
};

namespace BezBios {
namespace Sched {
template<typename T>
class ForYield {
private:
	volatile T value;
	ThreadControlBlock * for_task;
	ThreadControlBlock * yield_task;

public:
	ForYield() :
			value(nullptr), for_task(nullptr), yield_task(nullptr) {
	}
	;
	void connect(ThreadControlBlock * remote) {
		yield_task = remote;
		for_task = bezbios_sched_get_tid();
	}
	T future_data() {
		bezbios_sched_task_ready(for_task,0);
		bezbios_sched_switch_context(yield_task);
		return value;
	}
	void yield_data(T val) {
		value = val;
		bezbios_sched_task_ready(yield_task,0);
		bezbios_sched_switch_context(for_task);
	}
};

template<long SIZE>
class Bitfield{
	typedef volatile unsigned int CTRT;
	static constexpr long len = ((sizeof(CTRT)*8)-1+(SIZE))/(sizeof(CTRT)*8);
	CTRT field[len];
public:
	int get(long idx) volatile
	{
		int widx = idx / (8*sizeof(CTRT));
		int bit = idx  % (8*sizeof(CTRT));
		return !!(field[widx] & (1<<bit));
	}
	void set(long idx, int val) volatile
	{
		int widx = idx / (8*sizeof(CTRT));
		int bit = idx  % (8*sizeof(CTRT));
		if (val)
			field[widx]|= (1<<bit);
		else
			field[widx]&= ~(1<<bit);
	}
};

class TCB_LIST : public List<TCB_LIST>
{
public:
	ThreadControlBlock * tcb;
	TCB_LIST() : List(true){};
	TCB_LIST(TCB_LIST * block) : List(block,false){};
};

class Mutex{
private:
	ThreadControlBlock * locked; //thread id, thread 0 cannot into mutexes
	Mutex * next;
	TCB_LIST wthreads;
public:
	void aquire();
	void release();
	void destroy_task(ThreadControlBlock * tid);
	Mutex();
};

extern Mutex * mutex_list_head;

class ConditionVariable{
private:
	ConditionVariable * next;
	TCB_LIST wthreads;
public:
	bool notify_all();
	void wait();
	void destroy_task(ThreadControlBlock * tid);
	ConditionVariable();
};

extern ConditionVariable * condition_variable_list_head;

class ConditionVariableSingle{
private:
	ThreadControlBlock * wait_tid;
public:
	ThreadControlBlock * notify();
	void wait();
};

}
}

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
