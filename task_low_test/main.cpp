/*
 * main.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

#include <sched/bezbios_sched_api.h>
#include <uart/bezbios_uart_api_low.h>

extern "C"
{
__attribute__((used))
int bezbios_main();
}

/*class Test
{
	int test;
	Test() { test=4;};
};*/

int second_stack[4096];

void second(void *)
{
	bezbios_low_write_serial('2');
	bezbios_sched_switch_context(0);
	bezbios_low_write_serial('4');
	bezbios_sched_switch_context(0);
}

int bezbios_main()
{
	int stid=bezbios_sched_create_task(second,
			&second_stack[sizeof(second_stack)/sizeof(*second_stack)-1],
			nullptr);
	bezbios_low_write_serial('1');
	bezbios_sched_switch_context(stid);
	bezbios_low_write_serial('3');
	bezbios_sched_switch_context(stid);
	return 0;
}
