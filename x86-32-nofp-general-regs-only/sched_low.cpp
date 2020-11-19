/*
 * sched.c
 *
 *  Created on: Nov 11, 2020
 *      Author: Korneliusz Osmenda
 */


#include <sched/bezbios_sched_api.h>
#include "sched_low.h"

#include <etl/array.h>

struct Threads_sp {
	void * stack;
	void (*entry)();
};

#include "thread_py.h"

static
__attribute__((used))
__attribute__((section(".debug_gdb_scripts")))
const char thread_py[] =
	"\x01" THREAD_PY_PATH "/thread.py";


static etl::array<Threads_sp,CONFIG_MAX_THREADS> global_threads_sp = {{(void *)1,nullptr},};
static int current_tid;

void bezbios_sched_switch_context(int nexttask)
{
	asm("cli");

	int tid = current_tid;
	current_tid = nexttask;

	void (*entry)() = global_threads_sp[current_tid].entry;
	global_threads_sp[current_tid].entry = nullptr;

	BezBios::Sched::m32ngro::
	switchcontext_int(&global_threads_sp[tid].stack,global_threads_sp[nexttask].stack, entry);
}
int bezbios_sched_get_tid()
{
	return current_tid;
}

int bezbios_sched_create_task(void(*entry)(),void * stackbottom)
{
	auto& gts = global_threads_sp;
	for(auto i = gts.begin(); i != gts.end();i++)
	{
		if (i->stack == nullptr)
		{
			i->entry = entry;
			i->stack = stackbottom;
			return i - global_threads_sp.begin();
		}
	}
	return -1;
}
void bezbios_sched_destroy_task(int tid)
{
	global_threads_sp[tid].stack = nullptr;
}

