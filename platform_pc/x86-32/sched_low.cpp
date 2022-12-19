/*
 * sched.c
 *
 *  Created on: Nov 11, 2020
 *      Author: Korneliusz Osmenda
 */


#include <sched/bezbios_sched_api.h>
#include "sched_low.h"
#include "io.h"

struct Threads_sp {
	void * stack;
	void (*entry)(void *);
	void * val;
};

#include "thread_py.h"

static
__attribute__((used))
__attribute__((section(".debug_gdb_scripts")))
const char thread_py[] =
	"\x01" THREAD_PY_PATH "/thread.py";


__attribute__((externally_visible))
volatile Threads_sp global_threads_sp[CONFIG_MAX_THREADS] = {{(void *)1,nullptr,nullptr},};
static volatile int current_tid;

void bezbios_sched_switch_context(int nexttask)
{
	asm volatile("cli");

	int tid = current_tid;
	current_tid = nexttask;

	auto entry = global_threads_sp[current_tid].entry;
	global_threads_sp[current_tid].entry = nullptr;

	BezBios::Sched::m32ngro::
	switchcontext_int(&global_threads_sp[tid].stack,global_threads_sp[nexttask].stack, entry,global_threads_sp[nexttask].val);
}

int bezbios_sched_get_tid()
{
	return current_tid;
}

int bezbios_sched_create_task(void(*entry)(void*),void * stackbottom, void* val)
{
	auto& gts = global_threads_sp;
	for(long i = 0; i < CONFIG_MAX_THREADS ;i++)
	{
		if (gts[i].stack == nullptr)
		{
			gts[i].entry = entry;
			gts[i].stack = stackbottom;
			gts[i].val = val;
			return i;
		}
	}
	return -1;
}
void bezbios_sched_destroy_task(int tid)
{
	HWLOCK a;
	bezbios_sched_task_ready(tid,0);
	global_threads_sp[tid].stack = nullptr;
}

