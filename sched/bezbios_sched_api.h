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
int bezbios_sched_get_tid();
int bezbios_sched_create_task(void (*entry)(void *), void *stackbottom, void * val);
void bezbios_sched_destroy_task(int tid);


void bezbios_sched_task_ready(int tid, int is_ready);
void bezbios_sched_wfi(int interrupt);
void bezbios_sched_interrupt_handled(int interrupt);
int bezbios_sched_free_cpu();

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
		asm("cli");
		int tmp = task;
		task = stask;
		stask = tmp;
		asm("sti");
		return value;

	}
	void yield_data(T val) {
		while (stask == bezbios_sched_get_tid()) {
			bezbios_sched_switch_context(task);
		}
		value = val;
		asm("cli");
		int tmp = task;
		task = stask;
		stask = tmp;
		asm("sti");
	}
};
}
}

#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
