/*
 * cxa_guard.c
 *
 *  Created on: 27 wrz 2021
 *      Author: nyuu
 */

#include <cstdint>
#include <sched/bezbios_sched_api.h>

static BezBios::Sched::Mutex init_mutex;

static bool init_has_run(uint64_t *guard_object)
{
	return (((uint8_t *)guard_object)[0] != 0);
}

static void set_init_has_run(uint64_t *guard_object)
{
	((uint8_t *)guard_object)[0] = 1;
}


extern "C" int __cxa_guard_acquire(uint64_t * guard_object)
{
	if(init_has_run(guard_object))
	{
		return 0;
	}
	init_mutex.aquire();

	if(init_has_run(guard_object))
	{
		init_mutex.release();
		return 0;
	}

	return 1;
}

extern "C" void __cxa_guard_release(uint64_t * guard_object) {

	set_init_has_run(guard_object);
	init_mutex.release();
}
