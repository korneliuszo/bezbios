/*
 * bezbios_sched_api.h
 *
 *  Created on: Nov 11, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef SCHED_BEZBIOS_SCHED_API_H_
#define SCHED_BEZBIOS_SCHED_API_H_

#define CONFIG_MAX_THREADS 15

#ifdef __cplusplus
extern "C" {
#endif

void bezbios_sched_switch_context(int nexttask);
void bezbios_sched_switch_context_exit(int nexttask);
int bezbios_sched_get_tid();
int bezbios_sched_create_task(void (*entry)(void *), void *stackbottom, void * val);
void bezbios_sched_destroy_task(int tid);


void bezbios_sched_task_ready(int tid, bool is_ready);
int bezbios_sched_free_cpu(bool reschedule);
void bezbios_sched_exit(int tid);

#ifdef __cplusplus
}

template<void(*fn)(void), long size>
class BEZBIOS_CREATE_PROCESS
{
private:
	int stack[size] = {};
	static void entry(void *)	 {
		fn();
		bezbios_sched_exit(bezbios_sched_get_tid());
	}
public:
	BEZBIOS_CREATE_PROCESS()
	{
		int stid=bezbios_sched_create_task(entry, &stack[size-1], this);
		bezbios_sched_task_ready(stid,1);
	}
};

namespace BezBios {
namespace Sched {
template<typename T>
class ForYield {
private:
	volatile T value;
	volatile int for_task;
	volatile int yield_task;

public:
	ForYield() :
			value(nullptr), for_task(-1), yield_task(-1) {
	}
	;
	void connect(int remote) {
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


class Mutex{
private:
	volatile Bitfield<CONFIG_MAX_THREADS> waiting;
	volatile int locked; //thread id, thread 0 cannot into mutexes
	Mutex * next;
public:
	void aquire();
	void release();
	void destroy_task(int tid);
	Mutex();
};

extern Mutex * mutex_list_head;

class ConditionVariable{
private:
	volatile Bitfield<CONFIG_MAX_THREADS> waiting;
	ConditionVariable * next;
public:
	bool notify_all();
	void wait();
	void destroy_task(int tid);
	ConditionVariable();
};

extern ConditionVariable * condition_variable_list_head;

}
}

#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
