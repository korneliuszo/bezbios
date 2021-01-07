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


void bezbios_sched_task_ready(int tid, int is_ready);
void bezbios_sched_wfi(int interrupt);
void bezbios_sched_interrupt_handled(int interrupt);
int bezbios_sched_free_cpu(int reschedule);
int bezbios_sched_free_cpu_exit();

#define BEZBIOS_CREATE_PROCESS(fn,size) \
		static int fn ## _stack[size]; \
		static void fn ## _entry(void *) \
		{ \
			fn(); \
			bezbios_sched_task_ready(bezbios_sched_get_tid(),0); \
			bezbios_sched_free_cpu_exit(); \
		} \
		__attribute((constructor)) \
		static \
		void fn ## _init() \
		{ \
			int stid=bezbios_sched_create_task(fn ## _entry, \
					&fn ## _stack[size-1], \
					nullptr); \
			bezbios_sched_task_ready(stid,1); \
		}

#ifdef __cplusplus
}

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
	CTRT field[((sizeof(CTRT)*8)-1+(SIZE))/(sizeof(CTRT)*8)];
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
public:
	void aquire();
	void release();
};

class ConditionVariable{
private:
	volatile Bitfield<CONFIG_MAX_THREADS> waiting;
public:
	bool notify_all();
	void wait();
};

}
}

#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
