/*
 * sched_main.cpp
 *
 *  Created on: Dec 8, 2020
 *      Author: Korneliusz Osmenda
 */

#include <sched/bezbios_sched_api.h>

extern "C" {

__attribute__((weak))
__attribute__((used))
void bezbios_main(void);
//void bezbios_main() __attribute__((weak, alias("_bezbios_main")));

}

void bezbios_main()
{

	while(1)
	{
		while(bezbios_sched_free_cpu());
		asm("hlt");
		asm("nop");
	}

	return;
}
