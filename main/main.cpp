/*
 * main.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

#include <sched/bezbios_sched_api.h>
#include <uart/bezbios_uart_api.h>
#include <interrupts.h>

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
		if (leters[i] == 0)
		{
			i=0;
			bezbios_delay_ms(1500);
		}
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
		bezbios_serial_send(*letter);
	}
	bezbios_sched_destroy_task(stid);
}

void third()
 {
	char leters[]="Bios\n";
	int i = 0;
	while(1)
	{
		if (leters[i] == 0)
		{
			i=0;
			bezbios_delay_ms(1000);
		}
		bezbios_serial_send(leters[i]);
		i++;
	}

 }

void loop()
 {

	while(1)
	{
		unsigned char c;
		c=bezbios_serial_recv();
		bezbios_serial_send(c);
	}

 }

BEZBIOS_CREATE_PROCESS(loop,4096)
BEZBIOS_CREATE_PROCESS(first,4096)
BEZBIOS_CREATE_PROCESS(third,4096)
BEZBIOS_INIT_PIT()
BEZBIOS_INIT_SERIAL()
