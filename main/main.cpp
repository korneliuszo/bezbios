/*
 * main.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

#include <sched/bezbios_sched_api.h>
#include <uart/bezbios_uart_api.h>

extern "C"
{
int bezbios_main();
}

/*class Test
{
	int test;
	Test() { test=4;};
};*/

int second_stack[4096];
char leters[]= "1234";

static BezBios::Sched::ForYield<char *> yield;

void second()
{
	int i = 0;
	while(1)
	{
		yield.yield_data(&leters[i++]);
	}
}

int bezbios_main()
{
	int stid=bezbios_sched_create_task(second,
			&second_stack[sizeof(second_stack)/sizeof(*second_stack)-1]);

	yield.connect(stid);

	for (char * letter = yield.future_data();*letter;letter = yield.future_data())
		bezbios_low_write_serial(*letter);
	bezbios_sched_destroy_task(stid);
	return 0;
}
