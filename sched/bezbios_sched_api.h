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
int bezbios_sched_free_cpu();
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
	volatile int task;
	volatile int stask;

public:
	ForYield() :
			value(nullptr), task(-1), stask(-1) {
	}
	;
	void connect(int remote) {
		task = remote;
		stask = bezbios_sched_get_tid();
	}
	T future_data() {
		while (stask == bezbios_sched_get_tid()) {
			bezbios_sched_switch_context(task);
		}
		asm volatile("cli");
		int tmp = task;
		task = stask;
		stask = tmp;
		asm volatile("sti");
		return value;

	}
	void yield_data(T val) {
		while (stask == bezbios_sched_get_tid()) {
			bezbios_sched_switch_context(task);
		}
		value = val;
		asm volatile("cli");
		int tmp = task;
		task = stask;
		stask = tmp;
		asm volatile("sti");
	}
};

template<long SIZE>
class Bitfield{
	typedef unsigned int CTRT;
	CTRT field[((sizeof(CTRT)*8)-1+(SIZE))/(sizeof(CTRT)*8)];
public:
	int get(long idx)
	{
		int widx = idx / (8*sizeof(CTRT));
		int bit = idx  % (8*sizeof(CTRT));
		return !!(field[widx] & (1<<bit));
	}
	void set(long idx, int val)
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
	Bitfield<CONFIG_MAX_THREADS> waiting;
	int locked; //thread id, thread 0 cannot into mutexes
public:
	void aquire();
	void release();
};

}
}

#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
