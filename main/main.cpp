/*
 * main.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

#include <sched/bezbios_sched_api.h>
#include <uart/bezbios_uart_api.h>
#include <interrupts.h>
#include <uart/tlay2.hpp>

/*class Test
{
	int test;
	Test() { test=4;};
};*/

int second_stack[4096];

char leters[]= "Bez\n";

typedef BezBios::Sched::ForYield<char *> Yieldcptr;

void second(Yieldcptr *yield)
{
	int i = 0;
	while(1)
	{
		yield->yield_data(&leters[i]);
		if (leters[i] == 0)
		{
			i=0;
			bezbios_delay_ms(1500);
		}
		else
		{
			i++;
		}
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

	long i = 0;
	for (char * letter = yield.future_data();letter;letter = yield.future_data())
	{
		char buff[10];
		buff[i++] = *letter;
		if (*letter == 0)
		{
			tlay2_uart.dbgout(buff);
			i=0;
		}
	}
	bezbios_sched_destroy_task(stid);
}

void third()
 {
	char leters[]="Bios\n";
	while(1)
	{
		bezbios_delay_ms(1000);
		tlay2_uart.dbgout(leters);
	}

 }

BEZBIOS_CREATE_PROCESS(first,4096)
BEZBIOS_CREATE_PROCESS(third,4096)
BEZBIOS_INIT_PIT()
BEZBIOS_INIT_SERIAL()

struct Tlay2Payloads tlay2_payloads[] = {
		{1,tlay2_monitor},
		{0,nullptr} // terminator
};

