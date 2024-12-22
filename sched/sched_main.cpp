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
	bezbios_sched_create_task(&idle_tcb,nullptr,nullptr,nullptr);
	bezbios_sched_idle_it_is();
	bezbios_sched_task_ready(&idle_tcb,0); 
	asm("sti");
	while(1)
	{
		ThreadControlBlock * wait_tid;
		while((wait_tid = rr_next_task())!=&idle_tcb)
		{
				bezbios_sched_task_ready(wait_tid, 0);
				bezbios_sched_switch_context(wait_tid);
		}
		{
			asm("cli");
			if(rr_next_task()!=&idle_tcb)
			{
				asm("sti\n\t");
				continue;
			}
			asm(
				"sti\n\t"
				"hlt\n\t"
				"nop");
		}
	}

	return;
}
