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

char leters[]= "Bez";

typedef BezBios::Sched::ForYield<char *> Yieldcptr;

void second(Yieldcptr *yield)
{
	int i = 0;
	while(1)
	{
		if (leters[i] == 0)
			i=0;
		yield->yield_data(&leters[i]);
		i++;
	}
}

static
void first()
{
	Yieldcptr yield;
	int stid=bezbios_sched_create_task((void(*)(void*))second,
			&second_stack[4095],
			&yield);
	yield.connect(stid);

	for (char * letter = yield.future_data();*letter;letter = yield.future_data())
	{
		bezbios_low_write_serial(*letter);
		bezbios_sched_task_ready(bezbios_sched_get_tid(),1);
		bezbios_sched_free_cpu();
	}


	bezbios_sched_destroy_task(stid);
}

void third()
 {
	char leters[]="Bios";
	int i = 0;
	while(1)
	{
		if (leters[i] == 0)
			i=0;
		bezbios_low_write_serial(leters[i]);
		i++;
		bezbios_sched_task_ready(bezbios_sched_get_tid(),1);
		bezbios_sched_free_cpu();
	}

 }

BEZBIOS_CREATE_PROCESS(first,4096)
BEZBIOS_CREATE_PROCESS(third,4096)

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
