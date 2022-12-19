/*
 * sched_main.cpp
 *
 *  Created on: Dec 8, 2020
 *      Author: Korneliusz Osmenda
 */

#include <sched/bezbios_sched_api.h>
#include "io.h"

extern "C" {

__attribute__((weak))
__attribute__((used))
void bezbios_main(void);
//void bezbios_main() __attribute__((weak, alias("_bezbios_main")));

}

void bezbios_main()
{
	idle_tcb.plug(bezbios_sched_get_tid(), true);
	bezbios_sched_idle_it_is();
	while(1)
	{
		ThreadControlBlock * wait_tid;
		do {
			asm("cli");
			wait_tid = rr_next_task();
			if (wait_tid != &idle_tcb)
			{
				bezbios_sched_task_ready(wait_tid, 0);
				bezbios_sched_switch_context(wait_tid);
			}
		} while(wait_tid != &idle_tcb);
		{
			asm(
				"sti\n\t"
				"hlt\n\t"
				"nop");
		}
	}

	return;
}
