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

int first_stack[4096];
int second_stack[4096];

char leters[]= "1234";

typedef BezBios::Sched::ForYield<char *> Yieldcptr;

void second(Yieldcptr *yield)
{
	int i = 0;
	while(1)
	{
		yield->yield_data(&leters[i++]);
	}
}

void first(void *)
{
	Yieldcptr yield;
	int stid=bezbios_sched_create_task((void(*)(void*))second,
			&second_stack[4095],
			&yield);
	yield.connect(stid);

	for (char * letter = yield.future_data();*letter;letter = yield.future_data())
		bezbios_low_write_serial(*letter);

	bezbios_sched_destroy_task(stid);
	bezbios_sched_task_ready(bezbios_sched_get_tid(),0);
	bezbios_sched_free_cpu();
}

__attribute((constructor))
static
void first_init()
{
	int stid=bezbios_sched_create_task(first,
			&first_stack[4095],
			nullptr);

	bezbios_sched_task_ready(stid,1);
}

int bezbios_main()
{

	while(1)
	{
		while(bezbios_sched_free_cpu());
		asm("hlt");
		asm("nop");
	}

	return 0;
}
